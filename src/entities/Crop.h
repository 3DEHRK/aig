#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <nlohmann/json.hpp>

class ResourceManager;
class TileMap; // forward

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

private:
    TileMap* mapPtr = nullptr; // map for soil queries
    sf::RectangleShape shape; // placeholder visual
    std::string id;
    int maxStages;
    int currentStage = 0;
    float growth = 0.f;
    float totalGrowthTime;
    bool harvested = false;
    unsigned tileX = 0, tileY = 0; // location for fertility impact
    float droughtAccum = 0.f; // seconds moisture below threshold
    bool withered = false;
    bool finished = false; // ready for removal and tile freed
};