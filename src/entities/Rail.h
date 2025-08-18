#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>

class ResourceManager;

class Rail : public Entity {
public:
    Rail(ResourceManager& resources, const sf::Vector2f& pos, unsigned tileSize);
    void update(sf::Time) override {}
    void draw(sf::RenderWindow& win) override;
    sf::FloatRect getBounds() const override;
    void interact(Entity*) override {}
private:
    sf::RectangleShape shape;
};