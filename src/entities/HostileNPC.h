#pragma once
#include "NPC.h"

class Player;
class ResourceManager;

class HostileNPC : public NPC {
public:
    HostileNPC(ResourceManager& resources, const sf::Vector2f& pos, Player* target);
    void update(sf::Time dt) override;
    void interact(Entity* by) override;

    // health/damage API
    void takeDamage(float amount);
    bool isDead() const { return health <= 0.f; }
    float getHealth() const { return health; }
    float getMaxHealth() const { return maxHealth; }
private:
    Player* playerTarget = nullptr;
    float speed = 50.f;
    float attackRange = 24.f;
    float attackCooldown = 1.5f;
    float attackTimer = 0.f;

    // health
    float maxHealth = 10.f;
    float health = 10.f;
};