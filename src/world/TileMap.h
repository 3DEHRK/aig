#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

class TileMap {
public:
    enum Tile : uint8_t { Empty = 0, Solid = 1, Plantable = 2, Rail = 3 };

    TileMap(unsigned width = 50, unsigned height = 30, unsigned tileSize = 32u);
    void generateTestMap(); // simple demo layout
    void draw(sf::RenderWindow& window);
    void drawMoistureOverlay(sf::RenderWindow& window); // debug overlay: moisture alpha
    void drawFertilityOverlay(sf::RenderWindow& window); // debug overlay: fertility tint

    // query
    bool isTileSolid(unsigned tx, unsigned ty) const;
    bool isWorldPosSolid(const sf::Vector2f& worldPos) const;
    bool isRectColliding(const sf::FloatRect& rect) const;

    bool isTilePlantable(unsigned tx, unsigned ty) const { return inBounds(tx,ty) && tiles[tx + ty*w] == Plantable; }
    bool isTileRail(unsigned tx, unsigned ty) const { return inBounds(tx,ty) && tiles[tx + ty*w] == Rail; }
    Tile getTile(unsigned tx, unsigned ty) const { return inBounds(tx,ty) ? static_cast<Tile>(tiles[tx + ty*w]) : Solid; }

    void setTile(unsigned tx, unsigned ty, Tile t) { if (inBounds(tx,ty)) tiles[tx + ty*w] = t; }

    // exploration (fog-of-war for minimap)
    void markExplored(unsigned tx, unsigned ty) { if (inBounds(tx,ty)) explored[tx + ty*w] = 1; }
    bool isExplored(unsigned tx, unsigned ty) const { return inBounds(tx,ty) && explored[tx + ty*w] != 0; }

    unsigned width() const { return w; }
    unsigned height() const { return h; }
    unsigned tileSize() const { return ts; }
    sf::Vector2f worldSize() const { return {float(w*ts), float(h*ts)}; }

    // Soil system (basic)
    void updateSoil(sf::Time dt);
    float moisture(unsigned tx, unsigned ty) const { return inBounds(tx,ty)? soilMoisture[tx + ty*w] : 0.f; }
    float fertility(unsigned tx, unsigned ty) const { return inBounds(tx,ty)? soilFertility[tx + ty*w] : 0.f; }
    void addWater(unsigned tx, unsigned ty, float amt);
    void addFertility(unsigned tx, unsigned ty, float amt);
    void adjustFertility(unsigned tx, unsigned ty, float delta) { if (inBounds(tx,ty)) { soilFertility[tx+ty*w] = std::max(0.f,std::min(1.f, soilFertility[tx+ty*w] + delta)); } }

    nlohmann::json toJson() const {
        nlohmann::json j; j["w"]=w; j["h"]=h; j["ts"]=ts; j["tiles"]=tiles; j["soilMoisture"]=soilMoisture; j["soilFertility"]=soilFertility; j["explored"]=explored; return j; }
    void fromJson(const nlohmann::json& j) {
        if (!j.contains("w")||!j.contains("h")||!j.contains("ts")||!j.contains("tiles")) return;
        w=j["w"].get<unsigned>(); h=j["h"].get<unsigned>(); ts=j["ts"].get<unsigned>(); tiles=j["tiles"].get<std::vector<uint8_t>>();
        if (tiles.size()!=w*h) tiles.assign(w*h, Empty);
        soilMoisture = (j.contains("soilMoisture")? j["soilMoisture"].get<std::vector<float>>() : std::vector<float>(w*h,0.5f));
        soilFertility = (j.contains("soilFertility")? j["soilFertility"].get<std::vector<float>>() : std::vector<float>(w*h,0.5f));
        if (soilMoisture.size()!=w*h) soilMoisture.assign(w*h,0.5f);
        if (soilFertility.size()!=w*h) soilFertility.assign(w*h,0.5f);
        explored = (j.contains("explored")? j["explored"].get<std::vector<uint8_t>>() : std::vector<uint8_t>(w*h,0));
        if (explored.size()!=w*h) explored.assign(w*h,0);
    }

private:
    bool inBounds(unsigned tx, unsigned ty) const { return tx < w && ty < h; }
    unsigned w, h, ts;
    std::vector<uint8_t> tiles;

    // Soil arrays
    std::vector<float> soilMoisture; // 0..1
    std::vector<float> soilFertility; // 0..1
    // exploration array
    std::vector<uint8_t> explored; // 0=unseen,1=seen
};