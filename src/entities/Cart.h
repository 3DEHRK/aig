#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include "../items/Item.h"
class TileMap;
class ResourceManager;

class Cart : public Entity {
public:
    Cart(ResourceManager& res, const sf::Vector2f& pos, unsigned tileSize);
    void update(sf::Time dt) override;
    void draw(sf::RenderWindow& win) override; // non-const matches other entities
    sf::FloatRect getBounds() const override { return body.getGlobalBounds(); }
    void interact(Entity* by) override;
    void setTileMap(const TileMap* m) { map = m; }
    void setSpeed(float s) { speed = s; }
    void setLoop(bool v) { loopPath = v; }
    bool isLoop() const { return loopPath; } // added getter for persistence
    void addWaypoint(const sf::Vector2u& tile);
    void clearWaypoints() { waypoints.clear(); current = 0; }
    const std::vector<sf::Vector2u>& getWaypoints() const { return waypoints; }
    size_t currentIndex() const { return current; }
    // inventory
    bool addItem(const ItemPtr& it) {
        if (!it) return false;
        if (itemsCount() >= capacity) return false;
        contents.push_back(it); return true;
    }
    ItemPtr removeOne() {
        if (contents.empty()) return nullptr;
        ItemPtr it = contents.back(); contents.pop_back(); return it;
    }
    size_t itemsCount() const { return contents.size(); }
    size_t maxCapacity() const { return capacity; }
private:
    const TileMap* map = nullptr;
    std::vector<sf::Vector2u> waypoints;
    size_t current = 0;
    float speed = 60.f;
    bool loopPath = true;
    sf::RectangleShape body;
    sf::Vector2f targetPos;
    void advanceWaypoint();
    // simple inventory
    std::vector<ItemPtr> contents;
    size_t capacity = 16;
};