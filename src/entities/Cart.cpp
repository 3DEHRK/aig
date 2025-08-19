#include "Cart.h"
#include "Rail.h"
#include "../world/TileMap.h"
#include "../resources/ResourceManager.h"
#include "Player.h" // for rider control
#include <cmath>
#include <iostream>
#include <queue>
#include <unordered_set>

Cart::Cart(ResourceManager& res, const sf::Vector2f& pos, unsigned tileSize)
: sprite(res.texture("assets/textures/entities/tiles/cart.png"))
{
    body.setSize({tileSize * 0.6f, tileSize * 0.6f});
    body.setOrigin(body.getSize()*0.5f);
    body.setPosition(pos);
    body.setFillColor(sf::Color(200,180,60));
    const auto texSize = sprite.getTexture().getSize();
    float target = tileSize * 0.6f * 3.f; // 3x bigger than previous size
    float scale = target / static_cast<float>(texSize.x);
    sprite.setOrigin({texSize.x * 0.5f, texSize.y * 0.5f});
    sprite.setScale({scale, scale});
    sprite.setPosition(pos);
}

void Cart::addWaypoint(const sf::Vector2u& tile) {
    if (!map) { std::cerr << "Cart: no map set, cannot add waypoint.\n"; return; }
    // must be rail tile
    if (!map->isTileRail(tile.x, tile.y)) { std::cerr << "Waypoint rejected: not a rail tile ("<<tile.x<<","<<tile.y<<")\n"; return; }
    // if first waypoint, accept
    if (waypoints.empty()) {
        waypoints.push_back(tile);
        current = 0;
        float ts = (float)map->tileSize();
        targetPos = { tile.x * ts + ts*0.5f, tile.y * ts + ts*0.5f };
        body.setPosition(targetPos);
        sprite.setPosition(targetPos);
        return;
    }
    sf::Vector2u prev = waypoints.back();
    if (prev.x == tile.x && prev.y == tile.y) { std::cerr << "Waypoint duplicate ignored.\n"; return; }
    int dx = int(tile.x) - int(prev.x);
    int dy = int(tile.y) - int(prev.y);
    if (std::abs(dx) + std::abs(dy) != 1) { std::cerr << "Waypoint rejected: must be adjacent to previous (no skipping).\n"; return; }
    // adjacency implies connectivity since both tiles must be rail
    waypoints.push_back(tile);
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
    if (!map || waypoints.empty()) {
        if (rider) {
            rider->setPosition(body.getPosition());
        }
        sprite.setPosition(body.getPosition());
        // orientation when idle keeps last rotation
        return;
    }
    sf::Vector2f pos = body.getPosition();
    sf::Vector2f diff = targetPos - pos;
    float dist = std::sqrt(diff.x*diff.x + diff.y*diff.y);
    if (dist < 1.f) {
        advanceWaypoint();
        sprite.setPosition(body.getPosition());
        // recompute diff after advancing to adjust orientation
        diff = targetPos - body.getPosition();
        dist = std::sqrt(diff.x*diff.x + diff.y*diff.y);
    }
    if (dist > 0.f) diff /= dist;
    float step = speed * dt.asSeconds();
    if (step >= dist && dist>0.f) {
        body.setPosition(targetPos);
        advanceWaypoint();
    } else if (dist>0.f) {
        body.move(diff * step);
    }
    if (rider) {
        rider->setPosition(body.getPosition());
    }
    // orientation: rotate 90 deg for vertical movement (y-dominant)
    sf::Vector2f motion = targetPos - body.getPosition();
    if (std::abs(motion.x) > std::abs(motion.y)) sprite.setRotation(sf::degrees(0.f));
    else sprite.setRotation(sf::degrees(90.f));
    sprite.setPosition(body.getPosition());
    if (contents.empty()) body.setFillColor(sf::Color(200,180,60)); else body.setFillColor(sf::Color(200,120,40));
}

void Cart::draw(sf::RenderWindow& win) {
    win.draw(sprite);
}

void Cart::mount(Player* p) {
    if (!p) return; if (rider == p) return; rider = p; rider->setPosition(body.getPosition()); std::cerr << "Player mounted cart.\n"; }
void Cart::dismount() { if (!rider) return; std::cerr << "Player dismounted cart.\n"; rider = nullptr; }

void Cart::interact(Entity* by) {
    // toggle mount/dismount if player
    if (auto p = dynamic_cast<Player*>(by)) {
        if (!rider) mount(p); else dismount();
        return;
    }
    std::cerr << "Cart interaction placeholder.\n";
}
