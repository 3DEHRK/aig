#include "RailTool.h"
#include "../resources/ResourceManager.h"
#include "../world/TileMap.h"
#include <cmath>

RailTool::RailTool(ResourceManager& res, TileMap& m)
: resources(res), map(m) {}

void RailTool::update(const sf::Vector2f& worldPos, bool click) {
    unsigned ts = map.tileSize();
    hoverTile.x = (unsigned)std::floor(worldPos.x / (float)ts);
    hoverTile.y = (unsigned)std::floor(worldPos.y / (float)ts);
    if (hoverTile.x >= map.width() || hoverTile.y >= map.height()) return;

    if (click) {
        if (map.isTileRail(hoverTile.x, hoverTile.y)) {
            map.setTile(hoverTile.x, hoverTile.y, TileMap::Empty);
        } else if (!map.isTileSolid(hoverTile.x, hoverTile.y)) {
            map.setTile(hoverTile.x, hoverTile.y, TileMap::Rail);
        }
    }
}

void RailTool::drawPreview(sf::RenderWindow& win) {
    if (hoverTile.x >= map.width() || hoverTile.y >= map.height()) return;
    unsigned ts = map.tileSize();
    sf::RectangleShape r;
    r.setSize(sf::Vector2f((float)ts, (float)ts));
    r.setPosition(sf::Vector2f(hoverTile.x * ts, hoverTile.y * ts));
    r.setFillColor(sf::Color(0,0,0,0));
    r.setOutlineColor(sf::Color::Yellow);
    r.setOutlineThickness(1.f);
    win.draw(r);
}
