#pragma once
#include "NPC.h"

class Player;
class ResourceManager;

class HostileNPC : public NPC {
public:
    HostileNPC(ResourceManager& resources, const sf::Vector2f& pos, Player* target);
    void update(sf::Time dt) override;
    void interact(Entity* by) override;
private:
    Player* playerTarget = nullptr;
    float speed = 50.f;
    float attackRange = 24.f;
    float attackCooldown = 1.5f;
    float attackTimer = 0.f;
};
