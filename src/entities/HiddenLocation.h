#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>

class TileMap;
class Item;

class HiddenLocation : public Entity {
public:
    HiddenLocation(TileMap& map, unsigned tx, unsigned ty);
    void update(sf::Time dt) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
    void interact(Entity* by) override;

    bool discovered() const { return discoveredFlag; }
private:
    TileMap& map;
    unsigned tx, ty;
    bool discoveredFlag = false;
    sf::RectangleShape marker;
};
