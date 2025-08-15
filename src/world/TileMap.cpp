#include "TileMap.h"
#include <algorithm>
#include <cmath>
#include <type_traits>

TileMap::TileMap(unsigned width, unsigned height, unsigned tileSize)
: w(width), h(height), ts(tileSize), tiles(w*h, 0)
{}

void TileMap::generateTestMap() {
    std::fill(tiles.begin(), tiles.end(), 0);
    for (unsigned x = 0; x < w; ++x) {
        tiles[x + 0 * w] = 1;
        tiles[x + (h-1) * w] = 1;
    }
    for (unsigned y = 0; y < h; ++y) {
        tiles[0 + y * w] = 1;
        tiles[(w-1) + y * w] = 1;
    }
    for (unsigned x = 10; x < 15; ++x) tiles[x + 8*w] = 1;
    for (unsigned y = 12; y < 18; ++y) tiles[20 + y*w] = 1;
}

void TileMap::draw(sf::RenderWindow& window) {
    sf::RectangleShape r;
    r.setSize({float(ts), float(ts)});
    for (unsigned y = 0; y < h; ++y) {
        for (unsigned x = 0; x < w; ++x) {
            uint8_t t = tiles[x + y*w];
            r.setFillColor(t == 0 ? sf::Color(120,170,140) : sf::Color(60,60,60));
            r.setPosition(sf::Vector2f{float(x*ts), float(y*ts)});
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
    return tiles[tx + ty*w] != 0;
}

bool TileMap::isWorldPosSolid(const sf::Vector2f& worldPos) const {
    if (worldPos.x < 0 || worldPos.y < 0) return true;
    unsigned tx = static_cast<unsigned>(std::floor(worldPos.x)) / ts;
    unsigned ty = static_cast<unsigned>(std::floor(worldPos.y)) / ts;
    if (tx >= w || ty >= h) return true;
    return isTileSolid(tx, ty);
}

bool TileMap::isRectColliding(const sf::FloatRect& rect) const {
    for (unsigned ty = 0; ty < h; ++ty) {
        for (unsigned tx = 0; tx < w; ++tx) {
            if (!isTileSolid(tx, ty)) continue;
            sf::FloatRect tileRect(sf::Vector2f{float(tx * ts), float(ty * ts)},
                                   sf::Vector2f{float(ts), float(ts)});
            if (rectsIntersect(tileRect, rect)) return true;
        }
    }
    return false;
}