#pragma once
#include <SFML/Graphics.hpp>
#include "../world/TileMap.h"
#include <memory>

class ResourceManager;
class Game;

// Simple build-mode tool to preview and place/remove rail tiles
class RailTool {
public:
    RailTool(ResourceManager& res, TileMap& map);
    void toggle() { enabled = !enabled; }
    bool enabled = false;

    // call from PlayState: update with mouse/world coords and handle clicks
    void update(const sf::Vector2f& worldMouse, bool leftClick);
    void drawPreview(sf::RenderWindow& window);

private:
    ResourceManager& resources;
    TileMap& tileMap;
    sf::Vector2f hoverTileCenter;
    unsigned hoverTx = 0, hoverTy = 0;
    bool haveHover = false;
    std::unique_ptr<sf::Sprite> previewSprite; // use pointer because sf::Sprite has no default ctor here
    sf::RectangleShape previewRect;
};
