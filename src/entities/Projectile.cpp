#include "Projectile.h"
#include <algorithm>

Projectile::Projectile(const sf::Vector2f& pos, const sf::Vector2f& vel, float speed, float life, float dmg, float knock)
: velocity(vel), speedVal(speed), lifetime(life), damage(dmg), knockback(knock) {
    shape.setRadius(4.f);
    shape.setOrigin({4.f,4.f});
    shape.setFillColor(sf::Color::Yellow);
    shape.setPosition(pos);
}

void Projectile::update(sf::Time dt) {
    float s = dt.asSeconds();
    shape.move(velocity * s);
    lifetime -= s;
}

void Projectile::draw(sf::RenderWindow& win) { win.draw(shape); }

sf::FloatRect Projectile::getBounds() const { return shape.getGlobalBounds(); }
