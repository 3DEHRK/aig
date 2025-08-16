#include "RailTool.h"
#include "../resources/ResourceManager.h"
#include "../entities/Rail.h"

RailTool::RailTool(ResourceManager& res, TileMap& map)
: resources(res), tileMap(map), previewSprite(nullptr), previewRect() {
    previewRect.setSize({(float)map.tileSize(), (float)map.tileSize()});
    previewRect.setFillColor(sf::Color(120,120,120,120));
    previewRect.setOutlineColor(sf::Color::Yellow);
    previewRect.setOutlineThickness(2.f);
    try {
        sf::Texture& t = resources.texture("assets/textures/tiles/rail.png");
        previewSprite = std::make_unique<sf::Sprite>(t);
    } catch (...) {
        previewSprite = nullptr;
    }
}

void RailTool::update(const sf::Vector2f& worldMouse, bool leftClick) {
    int tx = static_cast<int>(std::floor(worldMouse.x)) / static_cast<int>(tileMap.tileSize());
    int ty = static_cast<int>(std::floor(worldMouse.y)) / static_cast<int>(tileMap.tileSize());
    if (tx < 0 || ty < 0 || static_cast<unsigned>(tx) >= tileMap.width() || static_cast<unsigned>(ty) >= tileMap.height()) {
        haveHover = false; return;
    }
    haveHover = true;
    hoverTx = (unsigned)tx; hoverTy = (unsigned)ty;
    hoverTileCenter = { (float)tx * (float)tileMap.tileSize() + tileMap.tileSize()*0.5f, (float)ty * (float)tileMap.tileSize() + tileMap.tileSize()*0.5f };
    previewRect.setPosition(sf::Vector2f((float)tx * tileMap.tileSize(), (float)ty * tileMap.tileSize()));
    if (previewSprite) previewSprite->setPosition(sf::Vector2f((float)tx * tileMap.tileSize(), (float)ty * tileMap.tileSize()));

    if (leftClick) {
        // toggle rail at tile
        if (tileMap.isTileRail(hoverTx, hoverTy)) tileMap.setTile(hoverTx, hoverTy, TileMap::Empty);
        else tileMap.setTile(hoverTx, hoverTy, TileMap::Rail);
    }
}

void RailTool::drawPreview(sf::RenderWindow& window) {
    if (!enabled || !haveHover) return;
    if (previewSprite) window.draw(*previewSprite);
    else window.draw(previewRect);
}

// Helper to create a Rail entity scaled to tile size
std::unique_ptr<Rail> createRailForTile(ResourceManager& res, TileMap& map, unsigned tx, unsigned ty) {
    sf::Vector2f pos((float)tx * map.tileSize(), (float)ty * map.tileSize());
    return std::make_unique<Rail>(res, pos, map.tileSize());
}
