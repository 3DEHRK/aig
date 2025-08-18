#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>

class Projectile : public Entity {
public:
    Projectile(const sf::Vector2f& pos, const sf::Vector2f& vel, float speed = 300.f, float life = 2.f);
    void update(sf::Time dt) override;
    void draw(sf::RenderWindow& win) override;
    sf::FloatRect getBounds() const override;
    void interact(Entity*) override {}
    bool expired() const { return lifetime <= 0.f; }
    void kill() { lifetime = 0.f; }
    float damage = 3.f;
private:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float speedVal;
    float lifetime;
};