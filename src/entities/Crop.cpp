#include "Crop.h"
#include "../resources/ResourceManager.h"
#include "../world/TileMap.h"
#include <iostream>
#include <cstdint>
#include <algorithm>

extern nlohmann::json* g_getTunablesJson();

// Static registry for crop configs
static std::unordered_map<std::string, CropConfig> g_cropConfigs;

void Crop::loadConfigs(ResourceManager& /*res*/, const std::string& path) {
    try {
        std::ifstream ifs(path);
        if (!ifs) { std::cerr << "[CropConfig] file not found: "<<path<<"\n"; return; }
        nlohmann::json j; ifs >> j;
        if (j.contains("crops") && j["crops"].is_array()) {
            for (auto &cj : j["crops"]) {
                if (!cj.contains("id")) continue;
                CropConfig cfg; cfg.id = cj["id"].get<std::string>();
                if (cj.contains("stage_durations")) for (auto &v : cj["stage_durations"]) cfg.stageDurations.push_back(v.get<float>());
                cfg.moistureIdealMin = cj.value("moisture_ideal_min", cfg.moistureIdealMin);
                cfg.moistureIdealMax = cj.value("moisture_ideal_max", cfg.moistureIdealMax);
                cfg.moistureDeathThreshold = cj.value("moisture_death_threshold", cfg.moistureDeathThreshold);
                cfg.moistureWitherSeconds = cj.value("moisture_wither_seconds", cfg.moistureWitherSeconds);
                cfg.fertilityYieldBonusScale = cj.value("fertility_yield_bonus_scale", cfg.fertilityYieldBonusScale);
                cfg.fertilityConsumption = cj.value("fertility_consumption", cfg.fertilityConsumption);
                cfg.baseYield = cj.value("base_yield", cfg.baseYield);
                cfg.maxQuality = cj.value("max_quality", cfg.maxQuality);
                if (cfg.stageDurations.empty()) {
                    // fallback to evenly split from tunables or default
                    if (auto *tj = g_getTunablesJson()) {
                        float base = cj.value("total_time", (*tj)["farming"].value("base_growth_seconds", nlohmann::json::object()).value(cfg.id, 6.f));
                        int stages = cj.value("stages", 3);
                        cfg.stageDurations.assign(stages, base / std::max(1, stages));
                    } else {
                        cfg.stageDurations.assign(3, 2.f);
                    }
                }
                g_cropConfigs[cfg.id] = cfg;
            }
            std::cerr << "[CropConfig] Loaded "<<g_cropConfigs.size()<<" configs from "<<path<<"\n";
        }
    } catch(std::exception &e) {
        std::cerr << "[CropConfig] error: "<<e.what()<<"\n";
    }
}

const CropConfig* Crop::getConfig(const std::string& id) {
    auto it = g_cropConfigs.find(id); if (it==g_cropConfigs.end()) return nullptr; return &it->second;
}

static float stageProgressFactor(const TileMap* map, unsigned tileX, unsigned tileY, const CropConfig* cfg) {
    if (!map) return 1.f;
    float m = map->moisture(tileX,tileY);
    float f = map->fertility(tileX,tileY);
    float mFactor = 1.f;
    if (cfg) {
        if (m < cfg->moistureIdealMin) {
            float span = std::max(0.01f, cfg->moistureIdealMin);
            mFactor = 0.3f + 0.7f * (m / span);
        } else if (m > cfg->moistureIdealMax) {
            float span = std::max(0.01f, 1.f - cfg->moistureIdealMax);
            mFactor = 1.f - 0.5f * ((m - cfg->moistureIdealMax)/span);
        }
    } else {
        // legacy heuristic
        float mFactorLegacy = (m < 0.4f) ? (0.5f + 0.5f * (m/0.4f)) : (m <= 0.7f ? 1.f : (1.f - (m-0.7f)*0.6f));
        if (mFactorLegacy < 0.3f) mFactorLegacy = 0.3f; mFactor = mFactorLegacy;
    }
    float fFactor = 0.4f + 0.6f * f;
    if (auto *tj = g_getTunablesJson()) {
        if ((*tj).contains("farming")) {
            auto &fj = (*tj)["farming"];
            mFactor *= fj.value("moisture_factor", 1.0f);
            fFactor *= fj.value("fertility_factor", 1.0f);
        }
    }
    return std::min(4.f, mFactor * fFactor);
}

Crop::Crop(ResourceManager& /*resources*/, TileMap& map, const sf::Vector2f& pos, const std::string& cropId, int stages, float totalTime)
: mapPtr(&map), id(cropId), maxStages(stages), totalGrowthTime(totalTime) {
    if (auto *tj = g_getTunablesJson()) {
        if ((*tj).contains("farming") && (*tj)["farming"].contains("base_growth_seconds")) {
            auto &bg = (*tj)["farming"]["base_growth_seconds"];
            if (bg.contains(id)) totalGrowthTime = bg[id].get<float>();
        }
    }
    shape.setSize({20.f,20.f});
    shape.setOrigin(shape.getSize()/2.f);
    shape.setFillColor(sf::Color(200,180,60));
    shape.setPosition(pos);
    unsigned ts = map.tileSize();
    tileX = (unsigned)std::floor(pos.x/ts); tileY = (unsigned)std::floor(pos.y/ts);
    const CropConfig* cfg = getConfig(id);
    if (cfg) {
        maxStages = (int)cfg->stageDurations.size();
        if (maxStages < 1) { maxStages = 1; }
    }
}

void Crop::update(sf::Time dt) {
    if (harvested || withered) return;
    const CropConfig* cfg = getConfig(id);
    if (mapPtr && cfg) {
        float m = mapPtr->moisture(tileX,tileY);
        if (m < cfg->moistureDeathThreshold) droughtAccum += dt.asSeconds();
        else droughtAccum = std::max(0.f, droughtAccum - dt.asSeconds()*0.5f);
        if (!withered && droughtAccum > cfg->moistureWitherSeconds) {
            withered = true; finished = true;
            sf::Color c = shape.getFillColor(); c.r=90; c.g=70; c.b=50; c.a=180; shape.setFillColor(c);
            std::cerr << "Crop withered: " << id << " at tile " << tileX << "," << tileY << "\n";
            return;
        }
    }
    float factor = stageProgressFactor(mapPtr, tileX, tileY, cfg);
    if (cfg) {
        // advance within current stage based on its duration
        float currentStageDuration = cfg->stageDurations[currentStage];
        growth += dt.asSeconds() * factor / std::max(0.001f, currentStageDuration);
        while (growth >= 1.f && currentStage < maxStages-1) {
            growth -= 1.f; // carry remainder
            int old = currentStage;
            currentStage++;
            sf::Color c = shape.getFillColor();
            c.r = (uint8_t)std::max(0, int(c.r) - 10);
            c.g = (uint8_t)std::max(0, int(c.g) - 5);
            float scale = 0.8f + (float)currentStage / std::max(1, maxStages-1) * 0.6f; // 0.8 .. 1.4
            shape.setSize({baseSize.x * scale, baseSize.y * scale});
            shape.setOrigin(shape.getSize()/2.f);
            shape.setFillColor(c);
            if (currentStage == maxStages-1) {
                c.g = (uint8_t)std::min(255, int(c.g) + 30);
                shape.setFillColor(c);
            }
        }
    } else {
        // legacy path
        float baseRate = 1.f / totalGrowthTime;
        growth += dt.asSeconds() * baseRate * factor * maxStages;
        growth = std::min(growth, (float)maxStages);
        int newStage = std::min(maxStages-1, (int)(growth / 1.f));
        if (newStage != currentStage) {
            currentStage = newStage;
            sf::Color c = shape.getFillColor();
            c.r = (uint8_t)std::max(0, int(c.r) - 10);
            c.g = (uint8_t)std::max(0, int(c.g) - 5);
            float scale = 0.8f + (float)currentStage / std::max(1, maxStages-1) * 0.6f;
            shape.setSize({baseSize.x * scale, baseSize.y * scale});
            shape.setOrigin(shape.getSize()/2.f);
            shape.setFillColor(c);
            if (currentStage == maxStages-1) { c.g = (uint8_t)std::min(255, int(c.g) + 30); shape.setFillColor(c); }
        }
    }
}

void Crop::draw(sf::RenderWindow& win) { if (!harvested) win.draw(shape); }

sf::FloatRect Crop::getBounds() const { return shape.getGlobalBounds(); }

void Crop::interact(Entity* /*by*/) {
    if (withered) { harvested = true; finished = true; return; }
    const CropConfig* cfg = getConfig(id);
    if (!harvested && currentStage == maxStages-1) {
        float fert = mapPtr ? mapPtr->fertility(tileX,tileY) : 0.5f;
        float scale = cfg? cfg->fertilityYieldBonusScale : 2.f;
        int baseY = cfg? cfg->baseYield : 1;
        float effective = baseY * (1.f + fert * scale);
        yield = std::max(1, (int)std::round(effective));
        // quality based on fertility threshold bands
        qualityTier = 1;
        if (cfg) {
            if (fert > 0.8f) qualityTier = std::min(cfg->maxQuality, 3);
            else if (fert > 0.55f) qualityTier = std::min(cfg->maxQuality, 2);
        } else {
            if (fert > 0.75f) qualityTier = 2;
        }
        harvested = true; finished = true; shape.setFillColor(sf::Color(120,120,120));
        if (mapPtr) mapPtr->adjustFertility(tileX,tileY, -(cfg? cfg->fertilityConsumption : 0.02f));
        std::cerr << "Crop harvested: " << id << " at tile " << tileX << "," << tileY << " yield=" << yield << " quality="<<qualityTier<<" fert=" << fert << "\n";
    }
}

nlohmann::json Crop::toJson() const {
    nlohmann::json j; sf::Vector2f p = shape.getPosition();
    j["id"]=id; j["x"]=p.x; j["y"]=p.y; j["maxStages"]=maxStages; j["currentStage"]=currentStage; j["growth"]=growth;
    j["totalGrowthTime"]=totalGrowthTime; j["harvested"]=harvested; j["tileX"]=tileX; j["tileY"]=tileY;
    j["droughtAccum"]=droughtAccum; j["withered"]=withered; j["finished"]=finished; j["yield"]=yield; j["quality"]=qualityTier; return j; }

std::unique_ptr<Crop> Crop::fromJson(ResourceManager& resources, TileMap& map, const nlohmann::json& j) {
    if (!j.contains("x")||!j.contains("y")||!j.contains("id")) return nullptr;
    std::string cid = j.value("id", "wheat"); int stages = j.value("maxStages",3); float ttime=j.value("totalGrowthTime",6.f);
    auto c = std::make_unique<Crop>(resources, map, sf::Vector2f(j["x"].get<float>(), j["y"].get<float>()), cid, stages, ttime);
    c->growth=j.value("growth",0.f); c->currentStage=j.value("currentStage",0); c->harvested=j.value("harvested",false);
    c->droughtAccum=j.value("droughtAccum",0.f); c->withered=j.value("withered",false); c->finished=j.value("finished", c->withered||c->harvested);
    c->tileX=j.value("tileX",0u); c->tileY=j.value("tileY",0u); c->yield=j.value("yield",1); c->qualityTier=j.value("quality",1);
    if (c->withered) { c->shape.setFillColor(sf::Color(90,70,50,180)); }
    else if (c->harvested) { c->shape.setFillColor(sf::Color(120,120,120)); }
    else { sf::Color base(200,180,60); for(int i=0;i<c->currentStage;i++){ base.r=(uint8_t)std::max(0, base.r-10); base.g=(uint8_t)std::max(0, base.g-5);} c->shape.setFillColor(base);} return c; }
