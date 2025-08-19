#pragma once
#include "NPC.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
extern nlohmann::json* g_getTunablesJson();
class Player; // forward

class HostileNPC : public NPC {
public:
    HostileNPC(const sf::Vector2f& pos, Player* target)
    : NPC(pos), playerTarget(target) {
        if (auto *tj = g_getTunablesJson()) {
            if ((*tj).contains("hostile") && (*tj)["hostile"].contains("grunt")) {
                auto &gj = (*tj)["hostile"]["grunt"];
                if (gj.contains("speed")) speed = gj["speed"].get<float>();
                if (gj.contains("health")) { maxHealth = gj["health"].get<float>(); health = maxHealth; }
                if (gj.contains("contact_damage")) contactDamage = gj["contact_damage"].get<float>();
            }
        }
    }

    void update(sf::Time dt) override; // implemented in cpp (handles movement + flash fade)
    void interact(Entity* /*by*/) override {}
    void takeDamage(float amount); // implemented in cpp
    void setHealth(float h) { health = std::max(0.f, std::min(maxHealth, h)); }
    void nudge(const sf::Vector2f& delta); // apply external displacement (knockback)
    void setTileMap(const TileMap* m) { tileMap = m; NPC::setTileMap(m); }

    // Health interface
    bool hasHealth() const override { return true; }
    float getHealth() const override { return health; }
    float getMaxHealth() const override { return maxHealth; }
    bool isDead() const override { return health <= 0.f; }
    void onDamaged(float) override { flashTimer = 0.15f; rageTimer = rageDuration; }
private:
    Player* playerTarget;
    float health = 10.f;
    float maxHealth = 10.f;
    float speed = 60.f;
    float attackRange = 48.f;
    float attackCooldown = 1.2f;
    float attackTimer = 0.f;
    float contactDamage = 8.f;
    float flashTimer = 0.f;

    // Rage mechanics
    float rageTimer = 0.f;
    float rageDuration = 0.f;
    float rageSpeedMult = 1.f;
    const TileMap* tileMap = nullptr; // for knockback collision tests

    // Loot stub (phase 2): simple drop chances
    float dropFiberChance = 0.6f; // 60% chance
    float dropCrystalChance = 0.1f; // 10% chance
};