#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>
#include "../input/InputManager.h"
#include "../systems/Inventory.h"

class ResourceManager;

class Player : public Entity {
public:
    Player(InputManager& input, ResourceManager& resources);
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

    // projectiles fired by player
    std::vector<std::unique_ptr<Entity>> projectiles;

    // fire a projectile in a direction (normalized)
    void fireProjectile(const sf::Vector2f& dir);
private:
    sf::Sprite sprite;
    float speed;
    mutable sf::Vector2f vel; // mutable so computeDesiredMove can be const
    InputManager& input;
    Inventory inv; // player's inventory
    bool interactPressed = false;
};