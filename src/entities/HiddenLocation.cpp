#include "HiddenLocation.h"
#include "../world/TileMap.h"
#include "Player.h"
#include "../items/Item.h"
#include <iostream>

HiddenLocation::HiddenLocation(TileMap& mapRef, unsigned tx_, unsigned ty_)
: map(mapRef), tx(tx_), ty(ty_)
{
    float ts = (float)map.tileSize();
    marker.setSize(sf::Vector2f(ts * 0.8f, ts * 0.8f));
    marker.setOrigin(marker.getSize() / 2.f);
    marker.setFillColor(sf::Color(80, 40, 140, 180));
    marker.setOutlineColor(sf::Color::Yellow);
    marker.setOutlineThickness(1.f);
    float px = tx * ts + ts * 0.5f;
    float py = ty * ts + ts * 0.5f;
    marker.setPosition(sf::Vector2f(px, py));
}

void HiddenLocation::update(sf::Time) {
    // nothing dynamic for now
}

void HiddenLocation::draw(sf::RenderWindow& window) {
    if (discoveredFlag) return; // only draw marker when undiscovered
    window.draw(marker);
}

sf::FloatRect HiddenLocation::getBounds() const {
    return marker.getGlobalBounds();
}

void HiddenLocation::interact(Entity* by) {
    if (discoveredFlag) return;
    if (auto p = dynamic_cast<Player*>(by)) {
        // mark discovered on tilemap
        map.setDiscovered(tx, ty, true);
        discoveredFlag = true;
        std::cerr << "Hidden location discovered at " << tx << "," << ty << "\n";
        // grant a rare dongle item for now
        auto dongle = std::make_shared<Item>("dongle_mysterious", "Mysterious Dongle", "A strange device. Could unlock something.", 1);
        if (p->inventory().addItem(dongle)) {
            std::cerr << "Player received mysterious dongle.\n";
        } else {
            std::cerr << "Player inventory full; dongle not added.\n";
        }
    }
}
