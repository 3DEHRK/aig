#include "RailTool.h"
#include "../resources/ResourceManager.h"
#include "../world/TileMap.h"
#include <cmath>
#include <iostream> // for feedback

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
            // enforce connectivity: unless this is the first rail overall, require adjacency to an existing rail
            bool anyRail = false;
            for (unsigned y=0; y<map.height() && !anyRail; ++y)
                for (unsigned x=0; x<map.width() && !anyRail; ++x)
                    if (map.isTileRail(x,y)) anyRail = true;
            bool adjacent = false;
            if (anyRail) {
                unsigned tx = hoverTile.x, ty = hoverTile.y;
                if (tx>0 && map.isTileRail(tx-1,ty)) adjacent = true;
                if (tx+1<map.width() && map.isTileRail(tx+1,ty)) adjacent = true;
                if (ty>0 && map.isTileRail(tx,ty-1)) adjacent = true;
                if (ty+1<map.height() && map.isTileRail(tx,ty+1)) adjacent = true;
            }
            if (!anyRail || adjacent) {
                map.setTile(hoverTile.x, hoverTile.y, TileMap::Rail);
            } else {
                std::cerr << "Cannot place rail: must connect to existing rail network.\n";
            }
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
