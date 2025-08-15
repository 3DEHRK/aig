#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

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

private:
    unsigned w, h, ts;
    std::vector<uint8_t> tiles; // 0 = empty, 1 = solid
};