#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

class TileMap {
public:
    TileMap(unsigned width = 50, unsigned height = 30, unsigned tileSize = 32u);
    void generateTestMap(); // simple demo layout
    void draw(sf::RenderWindow& window);

    // query
    bool isTileSolid(unsigned tx, unsigned ty) const;
    bool isWorldPosSolid(const sf::Vector2f& worldPos) const;
    bool isRectColliding(const sf::FloatRect& rect) const;

    unsigned width() const { return w; }
    unsigned height() const { return h; }
    unsigned tileSize() const { return ts; }
    sf::Vector2f worldSize() const { return {float(w*ts), float(h*ts)}; }

    // tile type helpers
    enum TileType : uint8_t { Empty = 0, Solid = 1, Soil = 2, Rail = 3 };
    uint8_t getTile(unsigned tx, unsigned ty) const;
    void setTile(unsigned tx, unsigned ty, uint8_t type);
    bool isTilePlantable(unsigned tx, unsigned ty) const; // true for Soil tiles
    bool isWorldPosPlantable(const sf::Vector2f& worldPos) const;
    bool isTileRail(unsigned tx, unsigned ty) const;
    bool isWorldPosRail(const sf::Vector2f& worldPos) const;

    // serialization helpers (JSON)
    nlohmann::json toJson() const;
    void fromJson(const nlohmann::json& j);

private:
    unsigned w, h, ts;
    std::vector<uint8_t> tiles; // 0 = empty, 1 = solid, 2 = soil, 3 = rail
};