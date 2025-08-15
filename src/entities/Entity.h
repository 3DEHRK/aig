#pragma once
#include <SFML/Graphics.hpp>

class Entity {
public:
    virtual ~Entity() = default;
    virtual void update(sf::Time) = 0;
    virtual void draw(sf::RenderWindow&) = 0;
    virtual sf::FloatRect getBounds() const = 0;
    virtual void interact(Entity* by) = 0;
};