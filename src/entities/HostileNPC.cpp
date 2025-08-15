#include "HostileNPC.h"
#include "../resources/ResourceManager.h"
#include "Player.h"
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
        // apply movement by moving underlying NPC sprite/shape
        if (sprite) sprite->move(move);
        else shape.move(move);
    } else {
        // in range: attack if cooldown elapsed
        attackTimer -= ds;
        if (attackTimer <= 0.f) {
            attackTimer = attackCooldown;
            std::cerr << "HostileNPC attacks player!\n";
            // rudimentary damage: try removing an item from player inventory (if any)
            if (!playerTarget->inventory().items().empty()) {
                auto it = playerTarget->inventory().items().front();
                if (it) {
                    playerTarget->inventory().removeItemById(it->id, 1);
                    std::cerr << "HostileNPC destroyed one " << it->id << " from player inventory.\n";
                }
            }
        }
    }
}

void HostileNPC::interact(Entity* by) {
    // hostile NPC will not engage in dialog; allow attack to be provoked
}
