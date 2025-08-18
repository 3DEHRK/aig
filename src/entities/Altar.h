#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>

class ResourceManager;
class Player;

class Altar : public Entity {
public:
    Altar(ResourceManager& resources, const sf::Vector2f& pos);
    void update(sf::Time) override; // currently no behaviour
    void draw(sf::RenderWindow& win) override;
    sf::FloatRect getBounds() const override;
    void interact(Entity* by) override; // attempt activation

    void setRequiredItems(const std::vector<std::string>& items); // specify ritual requirements
    bool isActive() const { return active; }
    bool grantsRespawn() const { return active; }
    const std::vector<std::string>& getRequiredItems() const { return requiredItems; }
    void forceActive(bool a) { active = a; }
private:
    std::unique_ptr<sf::Sprite> sprite; // SFML3 sprite requires texture in ctor
    bool active = false;
    std::vector<std::string> requiredItems;
};