#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
class ResourceManager; // forward declare for texture access

class TileMap {
public:
    enum Tile : uint8_t { Empty = 0, Solid = 1, Plantable = 2, Rail = 3 };

    TileMap(unsigned width = 50, unsigned height = 30, unsigned tileSize = 32u);
    void generateTestMap(); // simple demo layout
    void draw(sf::RenderWindow& window, bool showRailOverlay = true);
    void drawMoistureOverlay(sf::RenderWindow& window); // debug overlay: moisture alpha
    void drawFertilityOverlay(sf::RenderWindow& window); // debug overlay: fertility tint

    // query
    bool isTileSolid(unsigned tx, unsigned ty) const;
    bool isWorldPosSolid(const sf::Vector2f& worldPos) const;
    bool isRectColliding(const sf::FloatRect& rect) const;

    bool isTilePlantable(unsigned tx, unsigned ty) const { return inBounds(tx,ty) && tiles[tx + ty*w] == Plantable; }
    bool isTileRail(unsigned tx, unsigned ty) const { return inBounds(tx,ty) && tiles[tx + ty*w] == Rail; }
    Tile getTile(unsigned tx, unsigned ty) const { return inBounds(tx,ty) ? static_cast<Tile>(tiles[tx + ty*w]) : Solid; }

    void setTile(unsigned tx, unsigned ty, Tile t); // now updates rail connections
    uint8_t railBits(unsigned tx, unsigned ty) const { return (inBounds(tx,ty) && railMeta.size()==w*h) ? railMeta[tx + ty*w] : 0; }
    // bit layout: 1=N,2=E,4=S,8=W

    // exploration (fog-of-war for minimap)
    void markExplored(unsigned tx, unsigned ty) { if (explored.empty()) explored.assign(w*h,0); if (inBounds(tx,ty)) explored[tx + ty*w] = 1; }
    bool isExplored(unsigned tx, unsigned ty) const { return inBounds(tx,ty) && !explored.empty() && explored[tx + ty*w] != 0; }

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
    void setSoilTunables(float moistureTarget, float moistureDecayPerSec, float fertilityTarget, float fertilityRegenPerSec) {
        soilMoistureTarget = moistureTarget; soilMoistureDecay = moistureDecayPerSec; soilFertilityTarget = fertilityTarget; soilFertilityRegen = fertilityRegenPerSec; }

    void setMoistureDecayMultiplier(float m) { soilMoistureDecayMult = m; }
    float moistureDecayMultiplier() const { return soilMoistureDecayMult; }

    nlohmann::json toJson() const; // defined in cpp
    void fromJson(const nlohmann::json& j); // defined in cpp

    float moistureAt(unsigned tx, unsigned ty) const { if (!inBounds(tx,ty)) return 0.f; return soilMoisture[tx + ty*w]; }
    float fertilityAt(unsigned tx, unsigned ty) const { if (!inBounds(tx,ty)) return 0.f; return soilFertility[tx + ty*w]; }

    uint8_t railConnections(unsigned tx, unsigned ty) const { return railBits(tx,ty); }
    bool railHasNorth(unsigned tx, unsigned ty) const { return (railBits(tx,ty) & 1)!=0; }
    bool railHasEast(unsigned tx, unsigned ty) const { return (railBits(tx,ty) & 2)!=0; }
    bool railHasSouth(unsigned tx, unsigned ty) const { return (railBits(tx,ty) & 4)!=0; }
    bool railHasWest(unsigned tx, unsigned ty) const { return (railBits(tx,ty) & 8)!=0; }
    std::vector<sf::Vector2i> railExitOffsets(unsigned tx, unsigned ty) const;

    void setRailTexture(ResourceManager& res, const std::string& path); // new

private:
    void updateRailConnections(unsigned tx, unsigned ty); // recompute this rail & neighbor rails
    bool inBounds(unsigned tx, unsigned ty) const { return tx < w && ty < h; }
    unsigned w, h, ts;
    std::vector<uint8_t> tiles;
    std::vector<float> soilMoisture;
    std::vector<float> soilFertility;
    std::vector<uint8_t> explored;
    std::vector<uint8_t> railMeta; // parallel array storing connection bits for rails
    float soilMoistureTarget = 0.3f;
    float soilMoistureDecay = 0.02f; // per second toward target when above
    float soilFertilityTarget = 0.5f;
    float soilFertilityRegen = 0.005f; // per second when below target
    sf::Texture* railTexture = nullptr; // texture for rail tiles
    float soilMoistureDecayMult = 1.f; // new multiplier applied in updateSoil
};