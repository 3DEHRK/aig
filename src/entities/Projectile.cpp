#include "Projectile.h"

Projectile::Projectile(const sf::Vector2f& pos, const sf::Vector2f& vel, float dmg, Entity* own)
: velocity(vel), damage(dmg), owner(own)
{
    shape.setRadius(4.f);
    shape.setFillColor(sf::Color::Cyan);
    shape.setOrigin({4.f, 4.f});
    shape.setPosition(pos);
}

void Projectile::update(sf::Time dt) {
    float s = dt.asSeconds();
    shape.move(velocity * s);
    life -= s;
}

void Projectile::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

sf::FloatRect Projectile::getBounds() const { return shape.getGlobalBounds(); }

void Projectile::interact(Entity* by) { }
