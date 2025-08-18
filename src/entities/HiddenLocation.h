#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>

class TileMap;

class HiddenLocation : public Entity {
public:
    HiddenLocation(TileMap& map, unsigned tx, unsigned ty);
    void update(sf::Time) override {}
    void draw(sf::RenderWindow& win) override;
    sf::FloatRect getBounds() const override;
    void interact(Entity* by) override;
private:
    TileMap& tileMap;
    sf::Vector2f pos;
    bool discovered = false;
    sf::CircleShape marker;
};