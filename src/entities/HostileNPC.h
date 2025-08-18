#pragma once
#include "NPC.h"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>

class Player;

class HostileNPC : public NPC {
public:
    HostileNPC(const sf::Vector2f& pos, Player* target)
    : NPC(pos), playerTarget(target) {}

    void update(sf::Time dt) override {
        if (!playerTarget) return;
        float ds = dt.asSeconds();
        auto ppos = playerTarget->position();
        auto b = getBounds();
        sf::Vector2f center(b.position.x + b.size.x*0.5f, b.position.y + b.size.y*0.5f);
        float dx = ppos.x - center.x; float dy = ppos.y - center.y; float dist = std::sqrt(dx*dx + dy*dy);
        if (dist > attackRange) {
            if (dist > 0.f) {
                sf::Vector2f dir(dx/dist, dy/dist);
                shape.move(dir * speed * ds);
            }
        } else {
            attackTimer -= ds;
            if (attackTimer <= 0.f) {
                attackTimer = attackCooldown;
                std::cerr << "HostileNPC attacks player!\n";
                playerTarget->takeDamage(contactDamage);
            }
        }
    }

    void interact(Entity* /*by*/) override {}

    void takeDamage(float amount) {
        health -= amount;
        std::cerr << "HostileNPC took " << amount << " damage. health=" << health << "/" << maxHealth << "\n";
        if (health <= 0.f) {
            std::cerr << "HostileNPC died.\n";
        }
    }
    bool isDead() const { return health <= 0.f; }
    float getHealth() const { return health; }
    float getMaxHealth() const { return maxHealth; }
    void setHealth(float h) { health = std::max(0.f, std::min(maxHealth, h)); }
private:
    Player* playerTarget;
    float health = 10.f;
    float maxHealth = 10.f;
    float speed = 60.f;
    float attackRange = 48.f;
    float attackCooldown = 1.2f;
    float attackTimer = 0.f;
    float contactDamage = 8.f;
};