#include "TileMap.h"
#include <algorithm>
#include <cmath>
#include <type_traits>
#include <cstdint>
#include <nlohmann/json.hpp>

TileMap::TileMap(unsigned width, unsigned height, unsigned tileSize)
: w(width), h(height), ts(tileSize), tiles(w*h, Empty), soilMoisture(w*h, 0.5f), soilFertility(w*h,0.6f), explored(), railMeta(w*h,0) {}

void TileMap::generateTestMap() {
    std::fill(tiles.begin(), tiles.end(), Empty);
    // border walls
    for (unsigned x = 0; x < w; ++x) {
        tiles[x + 0 * w] = Solid;
        tiles[x + (h-1) * w] = Solid;
    }
    for (unsigned y = 0; y < h; ++y) {
        tiles[0 + y * w] = Solid;
        tiles[(w-1) + y * w] = Solid;
    }
    // sample interior obstacles
    for (unsigned x = 10; x < 15; ++x) tiles[x + 8*w] = Solid;
    for (unsigned y = 12; y < 18; ++y) tiles[20 + y*w] = Solid;

    // a small patch of plantable soil tiles near (6,6)
    for (unsigned y = 5; y <= 7; ++y) {
        for (unsigned x = 5; x <= 7; ++x) {
            tiles[x + y*w] = Plantable;
        }
    }
}

void TileMap::setTile(unsigned tx, unsigned ty, Tile t) {
    if (!inBounds(tx,ty)) return;
    tiles[tx + ty*w] = t;
    if (t == Rail) {
        if (railMeta.size()!=w*h) railMeta.assign(w*h,0);
        updateRailConnections(tx,ty);
        // also update neighbors to refresh their bitfields
        if (ty>0) updateRailConnections(tx,ty-1);
        if (ty+1<h) updateRailConnections(tx,ty+1);
        if (tx>0) updateRailConnections(tx-1,ty);
        if (tx+1<w) updateRailConnections(tx+1,ty);
    } else {
        if (railMeta.size()==w*h) {
            railMeta[tx + ty*w] = 0;
            // neighbors might lose a connection
            if (ty>0) updateRailConnections(tx,ty-1);
            if (ty+1<h) updateRailConnections(tx,ty+1);
            if (tx>0) updateRailConnections(tx-1,ty);
            if (tx+1<w) updateRailConnections(tx+1,ty);
        }
    }
}

void TileMap::updateRailConnections(unsigned tx, unsigned ty) {
    if (!inBounds(tx,ty)) return;
    if (railMeta.size()!=w*h) railMeta.assign(w*h,0);
    if (!isTileRail(tx,ty)) { railMeta[tx + ty*w] = 0; return; }
    uint8_t bits = 0;
    if (ty>0 && isTileRail(tx,ty-1)) bits |= 1; // N
    if (tx+1<w && isTileRail(tx+1,ty)) bits |= 2; // E
    if (ty+1<h && isTileRail(tx,ty+1)) bits |= 4; // S
    if (tx>0 && isTileRail(tx-1,ty)) bits |= 8; // W
    railMeta[tx + ty*w] = bits;
}

void TileMap::draw(sf::RenderWindow& window, bool showRailOverlay) {
    sf::RectangleShape r;
    r.setSize({float(ts), float(ts)});
    for (unsigned y = 0; y < h; ++y) {
        for (unsigned x = 0; x < w; ++x) {
            uint8_t t = tiles[x + y*w];
            switch (t) {
                case Empty: r.setFillColor(sf::Color(120,170,140)); break; // grass
                case Solid: r.setFillColor(sf::Color(60,60,60)); break; // rock
                case Plantable: {
                    float fert = soilFertility[x + y*w];
                    sf::Color base(150,110,60); sf::Color rich(180,140,90);
                    auto lerp=[&](uint8_t a,uint8_t b){ return uint8_t(a + (b-a)*fert); };
                    r.setFillColor(sf::Color(lerp(base.r,rich.r), lerp(base.g,rich.g), lerp(base.b,rich.b)));
                } break;
                case Rail: {
                    uint8_t bits = railBits(x,y);
                    int cnt = ((bits&1)!=0)+((bits&2)!=0)+((bits&4)!=0)+((bits&8)!=0);
                    sf::Color base(100,80,40);
                    if (cnt==1) base = sf::Color(110,90,50);
                    else if (cnt==2) base = sf::Color(125,105,60);
                    else if (cnt==3) base = sf::Color(140,120,70);
                    else if (cnt==4) base = sf::Color(160,140,80);
                    r.setFillColor(base);
                } break;
            }
            r.setPosition(sf::Vector2f{float(x*ts), float(y*ts)});
            window.draw(r);
            if (showRailOverlay && tiles[x + y*w] == Rail) {
                uint8_t bits = railBits(x,y);
                sf::Vector2f basePos(float(x*ts), float(y*ts));
                sf::Vertex lines[8]; int li=0;
                auto push=[&](sf::Vector2f a, sf::Vector2f b){
                    if (li+1 < 8) {
                        lines[li].position = a; lines[li].color = sf::Color::Black; ++li;
                        lines[li].position = b; lines[li].color = sf::Color::Black; ++li;
                    }
                };
                float cx = basePos.x + ts*0.5f; float cy = basePos.y + ts*0.5f;
                float len = ts*0.4f;
                if (bits & 1) push({cx,cy},{cx,cy-len});
                if (bits & 2) push({cx,cy},{cx+len,cy});
                if (bits & 4) push({cx,cy},{cx,cy+len});
                if (bits & 8) push({cx,cy},{cx-len,cy});
                if (li>0) window.draw(lines, li, sf::PrimitiveType::Lines);
            }
        }
    }
}

void TileMap::drawMoistureOverlay(sf::RenderWindow& window) {
    sf::RectangleShape r; r.setSize({float(ts), float(ts)});
    for (unsigned y=0;y<h;++y){
        for(unsigned x=0;x<w;++x){
            uint8_t t = tiles[x + y*w];
            if (t==Plantable){
                float m = soilMoisture[x + y*w];
                uint8_t alpha = static_cast<uint8_t>(std::clamp(m*255.f, 0.f, 255.f));
                r.setFillColor(sf::Color(40,100,220, alpha/2));
                r.setPosition({float(x*ts), float(y*ts)});
                window.draw(r);
            }
        }
    }
}

void TileMap::drawFertilityOverlay(sf::RenderWindow& window) {
    sf::RectangleShape r; r.setSize({float(ts), float(ts)});
    for (unsigned y=0;y<h;++y){
        for(unsigned x=0;x<w;++x){
            uint8_t t = tiles[x + y*w];
            if (t==Plantable){
                float f = soilFertility[x + y*w];
                // map fertility to green intensity and alpha
                uint8_t intensity = static_cast<uint8_t>(std::clamp(80.f + f*175.f, 0.f, 255.f));
                uint8_t alpha = static_cast<uint8_t>(std::clamp(40.f + f*180.f, 0.f, 255.f));
                r.setFillColor(sf::Color(30, intensity, 30, alpha/2));
                r.setPosition({float(x*ts), float(y*ts)});
                window.draw(r);
            }
        }
    }
}

// detect members on sf::FloatRect (cover SFML2: left/top/width/height, SFML3: position/size, fallbacks x/y/w/h)
template<typename R, typename = void>
struct has_member_left : std::false_type {};
template<typename R>
struct has_member_left<R, std::void_t<decltype(std::declval<const R&>().left)>> : std::true_type {};

template<typename R, typename = void>
struct has_member_top : std::false_type {};
template<typename R>
struct has_member_top<R, std::void_t<decltype(std::declval<const R&>().top)>> : std::true_type {};

template<typename R, typename = void>
struct has_member_width : std::false_type {};
template<typename R>
struct has_member_width<R, std::void_t<decltype(std::declval<const R&>().width)>> : std::true_type {};

template<typename R, typename = void>
struct has_member_height : std::false_type {};
template<typename R>
struct has_member_height<R, std::void_t<decltype(std::declval<const R&>().height)>> : std::true_type {};

template<typename R, typename = void>
struct has_member_position : std::false_type {};
template<typename R>
struct has_member_position<R, std::void_t<decltype(std::declval<const R&>().position)>> : std::true_type {};

template<typename R, typename = void>
struct has_member_size : std::false_type {};
template<typename R>
struct has_member_size<R, std::void_t<decltype(std::declval<const R&>().size)>> : std::true_type {};

template<typename R, typename = void>
struct has_member_x : std::false_type {};
template<typename R>
struct has_member_x<R, std::void_t<decltype(std::declval<const R&>().x)>> : std::true_type {};

template<typename R, typename = void>
struct has_member_y : std::false_type {};
template<typename R>
struct has_member_y<R, std::void_t<decltype(std::declval<const R&>().y)>> : std::true_type {};

template<typename R, typename = void>
struct has_member_w : std::false_type {};
template<typename R>
struct has_member_w<R, std::void_t<decltype(std::declval<const R&>().w)>> : std::true_type {};

template<typename R, typename = void>
struct has_member_h : std::false_type {};
template<typename R>
struct has_member_h<R, std::void_t<decltype(std::declval<const R&>().h)>> : std::true_type {};

// SFINAE-overloads to extract x,y,w,h from sf::FloatRect across SFML versions

// position/size (SFML3)
template<typename R, typename = std::void_t<decltype(std::declval<const R&>().position), decltype(std::declval<const R&>().size)>>
static void rect_to_xywh_impl(const R& r, float &x, float &y, float &w_, float &h_) {
    x = r.position.x;
    y = r.position.y;
    w_ = r.size.x;
    h_ = r.size.y;
}

// left/top/width/height (SFML2)
template<typename R, typename = std::void_t<decltype(std::declval<const R&>().left), decltype(std::declval<const R&>().top),
                                            decltype(std::declval<const R&>().width), decltype(std::declval<const R&>().height)>, 
         typename = void>
static void rect_to_xywh_impl(const R& r, float &x, float &y, float &w_, float &h_) {
    x = r.left;
    y = r.top;
    w_ = r.width;
    h_ = r.height;
}

// x/y/w/h fallback
template<typename R, typename = std::void_t<decltype(std::declval<const R&>().x), decltype(std::declval<const R&>().y),
                                            decltype(std::declval<const R&>().w), decltype(std::declval<const R&>().h)>, 
         typename = void, typename = void>
static void rect_to_xywh_impl(const R& r, float &x, float &y, float &w_, float &h_) {
    x = r.x;
    y = r.y;
    w_ = r.w;
    h_ = r.h;
}

// generic wrapper: calls the first viable impl
static void rect_to_xywh(const sf::FloatRect& r, float &x, float &y, float &w_, float &h_) {
    rect_to_xywh_impl(r, x, y, w_, h_);
}

static bool rectsIntersect(const sf::FloatRect& a, const sf::FloatRect& b) {
    float ax, ay, aw, ah;
    float bx, by, bw, bh;
    rect_to_xywh(a, ax, ay, aw, ah);
    rect_to_xywh(b, bx, by, bw, bh);
    return !(ax + aw <= bx || bx + bw <= ax || ay + ah <= by || by + bh <= ay);
}

bool TileMap::isTileSolid(unsigned tx, unsigned ty) const {
    if (tx >= w || ty >= h) return true;
    uint8_t t = tiles[tx + ty*w];
    return t == Solid; // only Solid blocks movement (Rails and Plantable are passable)
}

bool TileMap::isWorldPosSolid(const sf::Vector2f& worldPos) const {
    if (worldPos.x < 0 || worldPos.y < 0) return true;
    unsigned tx = static_cast<unsigned>(std::floor(worldPos.x)) / ts;
    unsigned ty = static_cast<unsigned>(std::floor(worldPos.y)) / ts;
    if (tx >= w || ty >= h) return true;
    return isTileSolid(tx, ty);
}

bool TileMap::isRectColliding(const sf::FloatRect& rect) const {
    float rx, ry, rw, rh; rect_to_xywh(rect, rx, ry, rw, rh);
    // treat out-of-bounds as colliding (keeps player inside world)
    float worldW = float(w * ts);
    float worldH = float(h * ts);
    if (rx < 0.f || ry < 0.f || (rx + rw) > worldW || (ry + rh) > worldH) return true;
    int minTx = std::max(0, int(std::floor(rx / ts)));
    int maxTx = std::min(int(w) - 1, int(std::floor((rx + rw - 0.0001f) / ts)));
    int minTy = std::max(0, int(std::floor(ry / ts)));
    int maxTy = std::min(int(h) - 1, int(std::floor((ry + rh - 0.0001f) / ts)));
    for (int ty = minTy; ty <= maxTy; ++ty) {
        for (int tx = minTx; tx <= maxTx; ++tx) {
            if (!isTileSolid(tx, ty)) continue;
            sf::FloatRect tileRect(sf::Vector2f{float(tx * ts), float(ty * ts)}, sf::Vector2f{float(ts), float(ts)});
            float txr, tyr, tww, thh; rect_to_xywh(tileRect, txr, tyr, tww, thh);
            if (!(rx + rw <= txr || txr + tww <= rx || ry + rh <= tyr || tyr + thh <= ry)) return true;
        }
    }
    return false;
}

void TileMap::updateSoil(sf::Time dt) {
    float ds = dt.asSeconds();
    for (size_t i=0;i<soilMoisture.size();++i) {
        float &m = soilMoisture[i];
        if (m > soilMoistureTarget) m = std::max(soilMoistureTarget, m - soilMoistureDecay * ds);
        else if (m < soilMoistureTarget) m = std::min(soilMoistureTarget, m + (soilMoistureDecay*0.5f) * ds);
    }
    float fertStep = soilFertilityRegen * ds;
    for (float &f : soilFertility) {
        if (f < soilFertilityTarget) f = std::min(soilFertilityTarget, f + fertStep);
    }
}

void TileMap::addWater(unsigned tx, unsigned ty, float amt) {
    if (!inBounds(tx,ty)) return;
    float &m=soilMoisture[tx+ty*w];
    m = std::min(1.f, m + amt);
}

void TileMap::addFertility(unsigned tx, unsigned ty, float amt) {
    if (!inBounds(tx,ty)) return;
    float &f=soilFertility[tx+ty*w];
    f = std::max(0.f, std::min(1.f, f + amt));
}

nlohmann::json TileMap::toJson() const {
    nlohmann::json j; j["w"]=w; j["h"]=h; j["ts"]=ts; j["tiles"]=tiles; j["soilMoisture"]=soilMoisture; j["soilFertility"]=soilFertility; j["explored"]=explored; if (railMeta.size()==w*h) j["railMeta"]=railMeta; return j; }
void TileMap::fromJson(const nlohmann::json& j) {
    if (!j.contains("w")||!j.contains("h")||!j.contains("ts")||!j.contains("tiles")) return;
    w=j["w"].get<unsigned>(); h=j["h"].get<unsigned>(); ts=j["ts"].get<unsigned>(); tiles=j["tiles"].get<std::vector<uint8_t>>();
    if (tiles.size()!=w*h) tiles.assign(w*h, Empty);
    soilMoisture = (j.contains("soilMoisture")? j["soilMoisture"].get<std::vector<float>>() : std::vector<float>(w*h,0.5f));
    soilFertility = (j.contains("soilFertility")? j["soilFertility"].get<std::vector<float>>() : std::vector<float>(w*h,0.5f));
    if (soilMoisture.size()!=w*h) soilMoisture.assign(w*h,0.5f);
    if (soilFertility.size()!=w*h) soilFertility.assign(w*h,0.5f);
    explored = (j.contains("explored")? j["explored"].get<std::vector<uint8_t>>() : std::vector<uint8_t>(w*h,0));
    if (explored.size()!=w*h) explored.assign(w*h,0);
    railMeta = (j.contains("railMeta")? j["railMeta"].get<std::vector<uint8_t>>() : std::vector<uint8_t>(w*h,0));
    if (railMeta.size()!=w*h) railMeta.assign(w*h,0);
    // recompute any missing rail bitfields if legacy save (railMeta missing but rails present)
    if (!j.contains("railMeta")) {
        for (unsigned y=0;y<h;++y) for(unsigned x=0;x<w;++x) if (isTileRail(x,y)) updateRailConnections(x,y);
    }
}

std::vector<sf::Vector2i> TileMap::railExitOffsets(unsigned tx, unsigned ty) const {
    std::vector<sf::Vector2i> v; if (!isTileRail(tx,ty)) return v; uint8_t b = railBits(tx,ty);
    if (b & 1) v.push_back({0,-1});
    if (b & 2) v.push_back({1,0});
    if (b & 4) v.push_back({0,1});
    if (b & 8) v.push_back({-1,0});
    return v;
}