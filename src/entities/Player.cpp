#include "Player.h"
#include "ItemEntity.h"
#include <algorithm>
#include <nlohmann/json.hpp>
#include "../resources/ResourceManager.h"
extern nlohmann::json* g_getTunablesJson();

Player::Player(InputManager& inputMgr, ResourceManager& res)
: speed(200.f), input(inputMgr), inv(32), health(100.f), maxHealth(100.f), regenRate(5.f), regenDelay(2.f), sinceDamage(0.f), invulnTimeRemaining(0.f), sprite(res.texture("assets/textures/entities/player_idle.png"))
{
    // apply tunables if present
    if (auto *tj = g_getTunablesJson()) {
        if ((*tj).contains("player")) {
            auto &pj = (*tj)["player"];
            if (pj.contains("speed")) speed = pj["speed"].get<float>();
            if (pj.contains("regen_rate")) regenRate = pj["regen_rate"].get<float>();
            if (pj.contains("regen_delay")) regenDelay = pj["regen_delay"].get<float>();
            if (pj.contains("base_damage")) damageBase = pj["base_damage"].get<float>();
            if (pj.contains("regen_curve_exponent")) regenCurveExponent = pj["regen_curve_exponent"].get<float>();
        }
    }
    shape.setSize({32.f, 32.f});
    shape.setFillColor(sf::Color::Green);
    shape.setOrigin(shape.getSize() / 2.f);
    shape.setPosition(sf::Vector2f{512.f, 384.f});
    // configure sprite
    auto texSize = sprite.getTexture().getSize();
    sprite.setOrigin({texSize.x * 0.5f, texSize.y * 0.5f});
    float scale = 32.f / texSize.x * 2.f; // doubled size
    sprite.setScale({scale, scale});
    sprite.setPosition(shape.getPosition());
}

void Player::update(sf::Time dt) {
    vel = {0.f, 0.f};
    // Action-based movement
    if (input.actionDown("MoveUp")) vel.y -= speed;
    if (input.actionDown("MoveDown")) vel.y += speed;
    if (input.actionDown("MoveLeft")) vel.x -= speed;
    if (input.actionDown("MoveRight")) vel.x += speed;
    if (input.actionPressed("Interact")) interactPressed = true;
    if (input.actionPressed("Inventory")) {/* inventory handled in state */}
    if (invulnTimeRemaining > 0.f) {
        invulnTimeRemaining -= dt.asSeconds();
        if (invulnTimeRemaining <= 0.f) { invulnTimeRemaining = 0.f; shape.setFillColor(sf::Color::Green); }
        else {
            sf::Color c = shape.getFillColor();
            if (static_cast<int>(invulnTimeRemaining * 20.f) % 2 == 0) c.a = 80; else c.a = 200;
            shape.setFillColor(c);
        }
    }
    if (damageFlashTimer > 0.f) {
        damageFlashTimer -= dt.asSeconds();
        float f = std::max(0.f, damageFlashTimer / 0.2f);
        sf::Color normal = sf::Color::Green;
        sf::Color hit = sf::Color(255,230,40);
        auto lerp=[&](uint8_t a,uint8_t b){ return uint8_t(a + (b-a)*f); };
        shape.setFillColor(sf::Color(lerp(normal.r, hit.r), lerp(normal.g, hit.g), lerp(normal.b, hit.b)));
        if (damageFlashTimer <= 0.f && invulnTimeRemaining <= 0.f) shape.setFillColor(normal);
    }
    // walk animation: rotate sprite slightly when moving
    float walkSpeed = std::sqrt(vel.x*vel.x + vel.y*vel.y);
    if (walkSpeed > 1.f) {
        walkAnim += dt.asSeconds() * 8.f; // speed up animation
        sprite.setRotation(sf::degrees(std::sin(walkAnim) * 10.f)); // swing ±10 deg
    } else {
        walkAnim = 0.f;
        sprite.setRotation(sf::degrees(0.f));
    }
    updateHealthRegen(dt);
}

sf::Vector2f Player::computeDesiredMove(sf::Time dt) const { return vel * dt.asSeconds(); }
void Player::applyMove(const sf::Vector2f& delta) { shape.move(delta); sprite.move(delta); }
void Player::setPosition(const sf::Vector2f& pos) { shape.setPosition(pos); sprite.setPosition(pos); }
sf::Vector2f Player::size() const { return shape.getSize(); }
void Player::draw(sf::RenderWindow& window) { window.draw(sprite); }
sf::FloatRect Player::getBounds() const { return shape.getGlobalBounds(); }
void Player::interact(Entity* other) { if (!other) return; if (auto itemEnt = dynamic_cast<ItemEntity*>(other)) itemEnt->interact(this); }
bool Player::wantsToInteract() const { return interactPressed; }
void Player::resetInteract() { interactPressed = false; }
sf::Vector2f Player::position() const { return shape.getPosition(); }
Inventory& Player::inventory() { return inv; }
void Player::updateHealthRegen(sf::Time dt) {
    sinceDamage += dt.asSeconds();
    if (health < maxHealth && sinceDamage >= regenDelay) {
        float rate = regenRate;
        if (regenCurveExponent > 0.f) {
            float missing = (maxHealth - health) / std::max(1.f, maxHealth);
            rate *= std::pow(missing, regenCurveExponent);
        }
        health = std::min(maxHealth, health + rate * dt.asSeconds());
    }
}
bool Player::hasWateringTool() const {
    for (auto &it : inv.items()) {
        if (it && it->id == "tool_wateringcan") return true;
    }
    return false;
}