#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>

class Projectile : public Entity {
public:
    Projectile(const sf::Vector2f& pos, const sf::Vector2f& vel, float speed = 300.f, float life = 2.f, float dmg = 3.f, float knock = 0.f);
    void update(sf::Time dt) override;
    void draw(sf::RenderWindow& win) override;
    sf::FloatRect getBounds() const override;
    void interact(Entity*) override {}
    bool expired() const { return lifetime <= 0.f; }
    float remainingLife() const { return lifetime; }
    void kill() { lifetime = 0.f; }
    const sf::Vector2f& getVelocity() const { return velocity; }
    float getKnockback() const { return knockback; }
    float damage = 3.f;
    float knockback = 0.f; // displacement magnitude applied to target on hit
private:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float speedVal;
    float lifetime;
};