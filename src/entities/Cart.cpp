#include "Cart.h"
#include "Rail.h"
#include "../world/TileMap.h"
#include "../resources/ResourceManager.h"
#include <cmath>
#include <iostream>

Cart::Cart(ResourceManager& /*res*/, const sf::Vector2f& pos, unsigned tileSize) {
    body.setSize({tileSize * 0.6f, tileSize * 0.6f});
    body.setOrigin(body.getSize()*0.5f);
    body.setPosition(pos);
    body.setFillColor(sf::Color(200,180,60));
}

void Cart::addWaypoint(const sf::Vector2u& tile) {
    waypoints.push_back(tile);
    if (waypoints.size()==1) {
        current = 0;
        if (map) {
            float ts = (float)map->tileSize();
            targetPos = { waypoints[0].x * ts + ts*0.5f, waypoints[0].y * ts + ts*0.5f };
            body.setPosition(targetPos);
        }
    }
}

void Cart::advanceWaypoint() {
    if (waypoints.empty()) return;
    if (current + 1 < waypoints.size()) ++current; else if (loopPath) current = 0; else return;
    if (map) {
        float ts = (float)map->tileSize();
        targetPos = { waypoints[current].x * ts + ts*0.5f, waypoints[current].y * ts + ts*0.5f };
    }
}

void Cart::update(sf::Time dt) {
    if (!map || waypoints.empty()) return;
    sf::Vector2f pos = body.getPosition();
    sf::Vector2f diff = targetPos - pos;
    float dist = std::sqrt(diff.x*diff.x + diff.y*diff.y);
    if (dist < 1.f) {
        advanceWaypoint();
        return;
    }
    if (dist > 0.f) diff /= dist;
    float step = speed * dt.asSeconds();
    if (step >= dist) {
        body.setPosition(targetPos);
        advanceWaypoint();
    } else {
        body.move(diff * step);
    }
    // cargo tint
    if (contents.empty()) body.setFillColor(sf::Color(200,180,60)); else body.setFillColor(sf::Color(200,120,40));
}

void Cart::draw(sf::RenderWindow& win) {
    win.draw(body);
}

void Cart::interact(Entity* /*by*/) {
    std::cerr << "Cart interaction placeholder.\n";
}
