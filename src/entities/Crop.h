#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <nlohmann/json.hpp>

class ResourceManager;
class TileMap; // forward

struct CropConfig { // new robust config
    std::string id;
    std::vector<float> stageDurations; // seconds per stage
    float moistureIdealMin = 0.35f;
    float moistureIdealMax = 0.65f;
    float moistureDeathThreshold = 0.15f; // prolonged below -> wither
    float moistureWitherSeconds = 25.f;
    float fertilityYieldBonusScale = 2.f; // multiplier scaling
    float fertilityConsumption = 0.02f; // on harvest
    int baseYield = 1;
    int maxQuality = 3; // quality tiers 1..maxQuality
};

class Crop : public Entity {
public:
    Crop(ResourceManager& resources, TileMap& map, const sf::Vector2f& pos, const std::string& cropId, int stages, float totalTime);
    void update(sf::Time dt) override;
    void draw(sf::RenderWindow& win) override;
    sf::FloatRect getBounds() const override;
    void interact(Entity* by) override;

    nlohmann::json toJson() const; // serialize full crop state
    static std::unique_ptr<Crop> fromJson(ResourceManager& resources, TileMap& map, const nlohmann::json& j); // construct + restore

    bool isFinished() const { return finished; }
    int yieldAmount() const { return yield; }
    bool wasHarvested() const { return harvested; }
    bool isWithered() const { return withered; }
    int quality() const { return qualityTier; }

    static void loadConfigs(ResourceManager& res, const std::string& path); // load JSON configs
    static const CropConfig* getConfig(const std::string& id);

private:
    TileMap* mapPtr = nullptr; // map for soil queries
    sf::RectangleShape shape; // placeholder visual
    std::string id;
    int maxStages;
    int currentStage = 0;
    float growth = 0.f; // normalized stage progress aggregated
    float totalGrowthTime; // legacy total (still used if no config)
    bool harvested = false;
    unsigned tileX = 0, tileY = 0; // location for fertility impact
    float droughtAccum = 0.f; // seconds moisture below threshold
    bool withered = false;
    bool finished = false; // ready for removal and tile freed
    int yield = 1; // base yield (can be modified by fertility)
    int qualityTier = 1; // new quality
    sf::Vector2f baseSize {20.f,20.f};
};