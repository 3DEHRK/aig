#pragma once
#include "Entity.h"
#include <memory>

class ResourceManager;

class Rail : public Entity {
public:
    Rail(ResourceManager& resources, const sf::Vector2f& pos);
    void update(sf::Time) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
    void interact(Entity* by) override;
private:
    bool useSprite = false;
    std::unique_ptr<sf::Sprite> sprite;
    sf::RectangleShape shape;
};
