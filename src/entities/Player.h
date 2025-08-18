#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include "../input/InputManager.h"
#include "../systems/Inventory.h"

class Projectile;

class Player : public Entity {
public:
    Player(InputManager& input);
    void update(sf::Time) override; // processes input and sets desired movement (but does not commit movement)
    void draw(sf::RenderWindow&) override;
    sf::FloatRect getBounds() const override;
    void interact(Entity* other) override;
    bool wantsToInteract() const;
    void resetInteract();
    sf::Vector2f position() const;

    // new helpers for collision-aware movement
    sf::Vector2f computeDesiredMove(sf::Time dt) const;
    void applyMove(const sf::Vector2f& delta);
    void setPosition(const sf::Vector2f& pos);

    // expose size so PlayState can build trial rectangles without touching FloatRect members
    sf::Vector2f size() const;

    Inventory& inventory(); // access player's inventory

    // Health interface
    float getHealth() const { return health; }
    float getMaxHealth() const { return maxHealth; }
    bool isDead() const { return health <= 0.f; }
    void takeDamage(float amt) { if (invulnTimeRemaining > 0.f) return; health = std::max(0.f, health - amt); sinceDamage = 0.f; damageAccumulatedThisLife += amt; }
    // track damage taken since last respawn for potential death penalties
    float damageThisLife() const { return damageAccumulatedThisLife; }
    void resetLifeStats() { damageAccumulatedThisLife = 0.f; }
    void healToFull() { health = maxHealth; sinceDamage = 0.f; }
    void updateHealthRegen(sf::Time dt);
    float timeSinceDamage() const { return sinceDamage; }

    // Projectile spawner placeholder (PlayState harvests this list)
    std::vector<std::unique_ptr<Entity>> projectiles;

    void triggerInvulnerability(float seconds) { invulnTimeRemaining = seconds; }
    bool isInvulnerable() const { return invulnTimeRemaining > 0.f; }
private:
    sf::RectangleShape shape;
    float speed;
    mutable sf::Vector2f vel; // mutable so computeDesiredMove can be const
    InputManager& input;
    Inventory inv; // player's inventory
    bool interactPressed = false;
    float health = 100.f;
    float maxHealth = 100.f;
    float regenRate = 2.f; // hp per second when conditions met
    float regenDelay = 5.f; // seconds after last damage
    float sinceDamage = 0.f;
    float invulnTimeRemaining = 0.f;
    float damageAccumulatedThisLife = 0.f;
};