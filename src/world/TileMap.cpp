#include "TileMap.h"
#include <algorithm>
#include <cmath>
#include <type_traits>
#include <iostream>

TileMap::TileMap(unsigned width, unsigned height, unsigned tileSize)
: w(width), h(height), ts(tileSize), tiles(w*h, TileMap::Empty) {}

void TileMap::generateTestMap() {
    // create borders
    for (unsigned x = 0; x < w; ++x) {
        setTile(x, 0, TileMap::Solid);
        setTile(x, h-1, TileMap::Solid);
    }
    for (unsigned y = 0; y < h; ++y) {
        setTile(0, y, TileMap::Solid);
        setTile(w-1, y, TileMap::Solid);
    }

    // create a patch of soil in the upper left
    for (unsigned y = 5; y < 9; ++y) {
        for (unsigned x = 4; x < 10; ++x) {
            setTile(x, y, TileMap::Soil);
        }
    }

    // create a small rail line
    unsigned rx = 6, ry = 18;
    for (unsigned i = 0; i < 6; ++i) {
        setTile(rx + i, ry, TileMap::Rail);
    }
}

void TileMap::draw(sf::RenderWindow& window) {
    // naive draw: draw colored rects for tiles
    for (unsigned y = 0; y < h; ++y) {
        for (unsigned x = 0; x < w; ++x) {
            uint8_t t = getTile(x, y);
            sf::RectangleShape r(sf::Vector2f((float)ts, (float)ts));
            r.setPosition(sf::Vector2f((float)x * ts, (float)y * ts));
            switch (t) {
                case TileMap::Solid: r.setFillColor(sf::Color(120, 120, 120)); break;
                case TileMap::Soil: r.setFillColor(sf::Color(160, 110, 60)); break;
                case TileMap::Rail: r.setFillColor(sf::Color(100, 100, 100)); break;
                default: r.setFillColor(sf::Color(80, 200, 120)); break;
            }
            window.draw(r);
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
    uint8_t t = tiles[ty * w + tx];
    return t == TileMap::Solid;
}

bool TileMap::isWorldPosSolid(const sf::Vector2f& worldPos) const {
    int tx = static_cast<int>(std::floor(worldPos.x)) / static_cast<int>(ts);
    int ty = static_cast<int>(std::floor(worldPos.y)) / static_cast<int>(ts);
    if (tx < 0 || ty < 0 || static_cast<unsigned>(tx) >= w || static_cast<unsigned>(ty) >= h) return true;
    return isTileSolid((unsigned)tx, (unsigned)ty);
}

bool TileMap::isRectColliding(const sf::FloatRect& rect) const {
    int x0 = static_cast<int>(std::floor(rect.position.x)) / static_cast<int>(ts);
    int y0 = static_cast<int>(std::floor(rect.position.y)) / static_cast<int>(ts);
    int x1 = static_cast<int>(std::floor(rect.position.x + rect.size.x - 1)) / static_cast<int>(ts);
    int y1 = static_cast<int>(std::floor(rect.position.y + rect.size.y - 1)) / static_cast<int>(ts);
    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            if (x < 0 || y < 0 || static_cast<unsigned>(x) >= w || static_cast<unsigned>(y) >= h) return true;
            if (isTileSolid((unsigned)x, (unsigned)y)) return true;
        }
    }
    return false;
}

uint8_t TileMap::getTile(unsigned tx, unsigned ty) const {
    if (tx >= w || ty >= h) return TileMap::Solid;
    return tiles[ty * w + tx];
}

void TileMap::setTile(unsigned tx, unsigned ty, uint8_t type) {
    if (tx >= w || ty >= h) return;
    tiles[ty * w + tx] = type;
}

bool TileMap::isTilePlantable(unsigned tx, unsigned ty) const {
    if (tx >= w || ty >= h) return false;
    return getTile(tx, ty) == TileMap::Soil;
}

bool TileMap::isWorldPosPlantable(const sf::Vector2f& worldPos) const {
    int tx = static_cast<int>(std::floor(worldPos.x)) / static_cast<int>(ts);
    int ty = static_cast<int>(std::floor(worldPos.y)) / static_cast<int>(ts);
    if (tx < 0 || ty < 0 || static_cast<unsigned>(tx) >= w || static_cast<unsigned>(ty) >= h) return false;
    return isTilePlantable((unsigned)tx, (unsigned)ty);
}

bool TileMap::isTileRail(unsigned tx, unsigned ty) const {
    if (tx >= w || ty >= h) return false;
    return getTile(tx, ty) == TileMap::Rail;
}

bool TileMap::isWorldPosRail(const sf::Vector2f& worldPos) const {
    int tx = static_cast<int>(std::floor(worldPos.x)) / static_cast<int>(ts);
    int ty = static_cast<int>(std::floor(worldPos.y)) / static_cast<int>(ts);
    if (tx < 0 || ty < 0 || static_cast<unsigned>(tx) >= w || static_cast<unsigned>(ty) >= h) return false;
    return isTileRail((unsigned)tx, (unsigned)ty);
}

nlohmann::json TileMap::toJson() const {
    nlohmann::json j;
    j["width"] = w;
    j["height"] = h;
    j["tileSize"] = ts;
    j["tiles"] = nlohmann::json::array();
    for (auto v : tiles) j["tiles"].push_back(v);
    return j;
}

void TileMap::fromJson(const nlohmann::json& j) {
    if (!j.contains("width") || !j.contains("height") || !j.contains("tiles")) return;
    unsigned nw = j["width"].get<unsigned>();
    unsigned nh = j["height"].get<unsigned>();
    auto arr = j["tiles"];
    if (!arr.is_array()) return;
    if (arr.size() != nw * nh) return;
    w = nw; h = nh;
    tiles.clear(); tiles.resize(w*h);
    for (size_t i = 0; i < arr.size(); ++i) tiles[i] = (uint8_t)arr[i].get<int>();
}