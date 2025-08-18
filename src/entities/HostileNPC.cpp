#include "HostileNPC.h"
#include "../resources/ResourceManager.h"
#include "Player.h"
#include "../world/TileMap.h"
#include <cmath>
#include <iostream>

HostileNPC::HostileNPC(ResourceManager& resources, const sf::Vector2f& pos, Player* target)
: NPC(resources, pos), playerTarget(target)
{
}

void HostileNPC::update(sf::Time dt) {
    if (!playerTarget) return;
    float ds = dt.asSeconds();
    // move towards player if further than attackRange
    auto ppos = playerTarget->position();
    // current position: use getBounds center
    auto b = getBounds();
    sf::Vector2f center(b.position.x + b.size.x*0.5f, b.position.y + b.size.y*0.5f);
    float dx = ppos.x - center.x;
    float dy = ppos.y - center.y;
    float dist = std::sqrt(dx*dx + dy*dy);
    if (dist > attackRange) {
        float nx = dx / dist; float ny = dy / dist;
        sf::Vector2f move(nx * speed * ds, ny * speed * ds);
        // collision-aware move using tileMap if available
        if (tileMap) {
            // attempt X move
            sf::FloatRect trialX(sf::Vector2f(center.x + move.x - b.size.x*0.5f, center.y - b.size.y*0.5f), sf::Vector2f(b.size.x, b.size.y));
            if (!tileMap->isRectColliding(trialX)) {
                if (sprite) sprite->move(sf::Vector2f(move.x, 0.f)); else shape.move(sf::Vector2f(move.x, 0.f));
            }
            // attempt Y move
            sf::FloatRect trialY(sf::Vector2f(center.x - b.size.x*0.5f, center.y + move.y - b.size.y*0.5f), sf::Vector2f(b.size.x, b.size.y));
            if (!tileMap->isRectColliding(trialY)) {
                if (sprite) sprite->move(sf::Vector2f(0.f, move.y)); else shape.move(sf::Vector2f(0.f, move.y));
            }
        } else {
            if (sprite) sprite->move(move);
            else shape.move(move);
        }
    } else {
        // in range: attack if cooldown elapsed
        attackTimer -= ds;
        if (attackTimer <= 0.f) {
            attackTimer = attackCooldown;
            std::cerr << "HostileNPC attacks player!\n";
            // apply damage to the player health instead of removing inventory items
            if (playerTarget) {
                playerTarget->takeDamage(2.f);
                std::cerr << "HostileNPC dealt 2 damage to player.\n";
            }
        }
    }
}

void HostileNPC::interact(Entity* by) {
    // hostile NPC will not engage in dialog; allow attack to be provoked
}

void HostileNPC::takeDamage(float amount) {
    health -= amount;
    std::cerr << "HostileNPC took " << amount << " damage. health=" << health << "/" << maxHealth << "\n";
    if (health <= 0.f) {
        std::cerr << "HostileNPC died.\n";
    }
}
