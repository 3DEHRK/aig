#include "HiddenLocation.h"
#include "../world/TileMap.h"
#include <iostream>

HiddenLocation::HiddenLocation(TileMap& map, unsigned tx, unsigned ty)
: tileMap(map) {
    float ts = (float)map.tileSize();
    pos = { tx * ts + ts*0.5f, ty * ts + ts*0.5f };
    marker.setRadius(6.f); marker.setOrigin({6.f,6.f}); marker.setFillColor(sf::Color(255,255,0,120)); marker.setPosition(pos);
}

void HiddenLocation::draw(sf::RenderWindow& win) {
    if (!discovered) win.draw(marker);
}

sf::FloatRect HiddenLocation::getBounds() const { return marker.getGlobalBounds(); }

void HiddenLocation::interact(Entity* /*by*/) {
    if (!discovered) { discovered = true; std::cerr << "Hidden location discovered!\n"; }
}






