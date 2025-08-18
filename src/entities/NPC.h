#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>
class TileMap;

class NPC : public Entity {
public:
    NPC(const sf::Vector2f& pos);
    void update(sf::Time) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
    void interact(Entity* by) override;
    void setTileMap(const TileMap* m) { tileMap = m; }
protected:
    sf::RectangleShape shape;
    const TileMap* tileMap = nullptr;
};