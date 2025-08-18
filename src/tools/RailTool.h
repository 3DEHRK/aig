#pragma once
#include <SFML/Graphics.hpp>
class ResourceManager;
class TileMap;

class RailTool {
public:
    RailTool(ResourceManager& resources, TileMap& map);
    void update(const sf::Vector2f& worldPos, bool click);
    void drawPreview(sf::RenderWindow& win);
    void toggle() { enabled = !enabled; }
    bool enabled = false;
private:
    ResourceManager& resources;
    TileMap& map;
    sf::Vector2u hoverTile{0,0};
};
