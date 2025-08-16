#pragma once
#include "Entity.h"
#include <memory>

class ResourceManager;

class Rail : public Entity {
public:
    // tileSize is used to scale the rail sprite to the grid size
    Rail(ResourceManager& resources, const sf::Vector2f& pos, unsigned tileSize = 32u);
    void update(sf::Time) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
    void interact(Entity* by) override;
private:
    unsigned tileSize = 32u;
    std::unique_ptr<sf::Sprite> sprite;
    sf::RectangleShape shape;
};
