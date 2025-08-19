#include "Crop.h"
#include "../resources/ResourceManager.h"
#include "../world/TileMap.h"
#include <iostream>
#include <cstdint>
#include <algorithm>

extern nlohmann::json* g_getTunablesJson();
static float growthFactor(const TileMap* map, unsigned tileX, unsigned tileY) {
    if (!map) return 1.f;
    float m = map->moisture(tileX,tileY);
    float f = map->fertility(tileX,tileY);
    float mFactor = (m < 0.4f) ? (0.5f + 0.5f * (m/0.4f)) : (m <= 0.7f ? 1.f : (1.f - (m-0.7f)*0.6f));
    if (mFactor < 0.3f) mFactor = 0.3f;
    float fFactor = 0.4f + 0.6f * f;
    float combined = mFactor * fFactor;
    if (auto *tj = g_getTunablesJson()) {
        if ((*tj).contains("farming")) {
            auto &fj = (*tj)["farming"];
            float moistureMul = fj.value("moisture_factor", 1.0f);
            float fertilityMul = fj.value("fertility_factor", 1.0f);
            combined = (mFactor * moistureMul) * (fFactor * fertilityMul);
        }
    }
    return combined;
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
}

void Crop::update(sf::Time dt) {
    if (harvested || withered) return;
    float baseRate = 1.f / totalGrowthTime;
    float factor = growthFactor(mapPtr, tileX, tileY);
    // apply clamping so extreme multipliers (future) don’t exceed 4x early-game sanity
    factor = std::min(factor, 4.f);
    if (mapPtr) {
        float m = mapPtr->moisture(tileX,tileY);
        if (m < 0.25f) droughtAccum += dt.asSeconds(); else droughtAccum = std::max(0.f, droughtAccum - dt.asSeconds()*0.5f);
        if (!withered && droughtAccum > 25.f) {
            withered = true; finished = true;
            sf::Color c = shape.getFillColor(); c.r=90; c.g=70; c.b=50; c.a=180; shape.setFillColor(c);
            std::cerr << "Crop withered: " << id << " at tile " << tileX << "," << tileY << "\n";
            return;
        }
    }
    growth += dt.asSeconds() * baseRate * factor * maxStages;
    growth = std::min(growth, (float)maxStages); // cap
    int newStage = std::min(maxStages-1, (int)(growth / 1.f));
    if (newStage != currentStage) {
        int old = currentStage;
        currentStage = newStage;
        sf::Color c = shape.getFillColor();
        c.r = (uint8_t)std::max(0, int(c.r) - 10);
        c.g = (uint8_t)std::max(0, int(c.g) - 5);
        // scale size slightly with stage
        float scale = 0.8f + (float)currentStage / std::max(1, maxStages-1) * 0.6f; // 0.8 .. 1.4
        shape.setSize({baseSize.x * scale, baseSize.y * scale});
        shape.setOrigin(shape.getSize()/2.f);
        shape.setFillColor(c);
        // future: OnStageAdvance hook
        if (currentStage == maxStages-1) {
            // ready to harvest tint
            c.g = (uint8_t)std::min(255, int(c.g) + 30);
            shape.setFillColor(c);
        }
    }
}

void Crop::draw(sf::RenderWindow& win) { if (!harvested) win.draw(shape); }

sf::FloatRect Crop::getBounds() const { return shape.getGlobalBounds(); }

void Crop::interact(Entity* /*by*/) {
    if (withered) { harvested = true; finished = true; return; }
    if (!harvested && currentStage == maxStages-1) {
        // compute yield influenced by fertility
        float fert = mapPtr ? mapPtr->fertility(tileX,tileY) : 0.5f;
        float bonusScale = 2.f;
        if (auto *tj = g_getTunablesJson()) {
            if ((*tj).contains("farming")) bonusScale = (*tj)["farming"].value("yield_bonus_scale", 2.f);
        }
        float effective = 1.f + fert * bonusScale; // 1..(1+bonusScale)
        yield = std::max(1, (int)std::round(effective));
        harvested = true; finished = true; shape.setFillColor(sf::Color(120,120,120));
        if (mapPtr) mapPtr->adjustFertility(tileX,tileY,-0.02f);
        std::cerr << "Crop harvested: " << id << " at tile " << tileX << "," << tileY << " yield=" << yield << " fert=" << fert << "\n";
        // TODO: integrate inventory drops; placeholder log only.
    }
}

nlohmann::json Crop::toJson() const {
    nlohmann::json j; sf::Vector2f p = shape.getPosition();
    j["id"]=id; j["x"]=p.x; j["y"]=p.y; j["maxStages"]=maxStages; j["currentStage"]=currentStage; j["growth"]=growth;
    j["totalGrowthTime"]=totalGrowthTime; j["harvested"]=harvested; j["tileX"]=tileX; j["tileY"]=tileY;
    j["droughtAccum"]=droughtAccum; j["withered"]=withered; j["finished"]=finished; return j; }

std::unique_ptr<Crop> Crop::fromJson(ResourceManager& resources, TileMap& map, const nlohmann::json& j) {
    if (!j.contains("x")||!j.contains("y")||!j.contains("id")) return nullptr;
    std::string cid = j.value("id", "wheat"); int stages = j.value("maxStages",3); float ttime=j.value("totalGrowthTime",6.f);
    auto c = std::make_unique<Crop>(resources, map, sf::Vector2f(j["x"].get<float>(), j["y"].get<float>()), cid, stages, ttime);
    c->growth=j.value("growth",0.f); c->currentStage=j.value("currentStage",0); c->harvested=j.value("harvested",false);
    c->droughtAccum=j.value("droughtAccum",0.f); c->withered=j.value("withered",false); c->finished=j.value("finished", c->withered||c->harvested);
    c->tileX=j.value("tileX",0u); c->tileY=j.value("tileY",0u);
    if (c->withered) { c->shape.setFillColor(sf::Color(90,70,50,180)); }
    else if (c->harvested) { c->shape.setFillColor(sf::Color(120,120,120)); }
    else { sf::Color base(200,180,60); for(int i=0;i<c->currentStage;i++){ base.r=(uint8_t)std::max(0, base.r-10); base.g=(uint8_t)std::max(0, base.g-5);} c->shape.setFillColor(base);} return c; }
