#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>

class Projectile : public Entity {
public:
    Projectile(const sf::Vector2f& pos, const sf::Vector2f& vel);
    void update(sf::Time dt) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
    void interact(Entity* by) override;

    bool expired() const { return life <= 0.f; }
    void kill() { life = 0.f; }
private:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float life = 2.0f; // seconds
};
