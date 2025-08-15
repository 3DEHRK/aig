#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>
#include "../items/Item.h"
#include <memory>
#include <vector>

class ResourceManager;

class Crop : public Entity {
public:
    Crop(ResourceManager& resources, const sf::Vector2f& pos, const std::string& seedId = "seed_wheat", int stages = 3, float secondsPerStage = 10.f);
    void update(sf::Time dt) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
    void interact(Entity* other) override;
    bool isHarvested() const { return harvested; }
private:
    int stage = 0;
    int maxStage = 3;
    float timer = 0.f;
    float secondsPerStage = 10.f;
    sf::RectangleShape shape;
    std::unique_ptr<sf::Sprite> sprite; // optional sprite when texture is available
    std::vector<const sf::Texture*> stageTextures;
    bool useSprite = false;
    ItemPtr yieldItem; // item produced when harvested
    bool harvested = false;
};
