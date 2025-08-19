#pragma once
#include "NPC.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
extern nlohmann::json* g_getTunablesJson();
class Player; // forward

class HostileNPC : public NPC {
public:
    enum Type { Grunt, Tank };
    HostileNPC(const sf::Vector2f& pos, Player* target, Type type = Grunt)
    : NPC(pos), playerTarget(target), variant(type) {
        if (auto *tj = g_getTunablesJson()) {
            if ((*tj).contains("hostile")) {
                auto &hroot = (*tj)["hostile"];
                const char* key = (variant==Tank? "tank" : "grunt");
                if (hroot.contains(key)) {
                    auto &gj = hroot[key];
                    if (gj.contains("speed")) speed = gj["speed"].get<float>();
                    if (gj.contains("health")) { maxHealth = gj["health"].get<float>(); health = maxHealth; }
                    if (gj.contains("contact_damage")) contactDamage = gj["contact_damage"].get<float>();
                }
            }
        }
        // tank visual tweak
        if (variant==Tank) { shape.setSize({40.f,40.f}); shape.setOrigin(shape.getSize()/2.f); shape.setFillColor(sf::Color(180,70,70)); }
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

    // Variant accessor for persistence
    Type getType() const { return variant; }
private:
    Player* playerTarget;
    Type variant = Grunt;
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