#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>

class Projectile : public Entity {
public:
    Projectile(const sf::Vector2f& pos, const sf::Vector2f& vel, float damage = 5.f, Entity* owner = nullptr);
    void update(sf::Time dt) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
    void interact(Entity* by) override;

    bool expired() const { return life <= 0.f; }
    void kill() { life = 0.f; }

    float getDamage() const { return damage; }
    Entity* getOwner() const { return owner; }
private:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float life = 2.0f; // seconds
    float damage = 5.f;
    Entity* owner = nullptr;
};
