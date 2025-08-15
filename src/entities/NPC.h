#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <memory>

class ResourceManager;
class TileMap;

class NPC : public Entity {
public:
    NPC(ResourceManager& resources, const sf::Vector2f& pos);
    void update(sf::Time) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
    void interact(Entity* by) override;

    // allow PlayState to provide a TileMap pointer for simple collision checks
    void setTileMap(TileMap* m) { tileMap = m; }
protected:
    sf::RectangleShape shape;
    std::unique_ptr<sf::Sprite> sprite;
    bool useSprite = false;
    TileMap* tileMap = nullptr;
};