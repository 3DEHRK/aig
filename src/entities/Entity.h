#pragma once
#include <SFML/Graphics.hpp>
#include <optional>

class Entity {
public:
    virtual ~Entity() = default;
    virtual void update(sf::Time) = 0;
    virtual void draw(sf::RenderWindow&) = 0;
    virtual sf::FloatRect getBounds() const = 0;
    virtual void interact(Entity* by) = 0;
    // Optional health interface (default: no health)
    virtual bool hasHealth() const { return false; }
    virtual float getHealth() const { return 0.f; }
    virtual float getMaxHealth() const { return 0.f; }
    virtual void applyDamage(float /*amt*/) {} // callers use this for generic damage
    virtual bool isDead() const { return false; }
    virtual void onDamaged(float /*amount*/) {}
};

// Simple AABB collision helper (SFML3 compatible)
inline bool aabbIntersect(const sf::FloatRect& a, const sf::FloatRect& b) {
    return !(a.position.x + a.size.x <= b.position.x || b.position.x + b.size.x <= a.position.x ||
             a.position.y + a.size.y <= b.position.y || b.position.y + b.size.y <= a.position.y);
}

// Sweep test along one axis: returns corrected delta so AABB doesn't penetrate obstacle bounds
inline float resolveAxis(float posMin, float posMax, float move, float blockMin, float blockMax) {
    if (move > 0.f) {
        if (posMax <= blockMin && posMax + move > blockMin) return blockMin - posMax; // clamp forward
    } else if (move < 0.f) {
        if (posMin >= blockMax && posMin + move < blockMax) return blockMax - posMin; // clamp backward
    }
    return move; // no adjustment
}