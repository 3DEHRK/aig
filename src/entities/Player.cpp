#include "Player.h"
#include "ItemEntity.h"
#include <algorithm>

Player::Player(InputManager& inputMgr)
: speed(200.f), input(inputMgr), inv(32), health(100.f), maxHealth(100.f), regenRate(5.f), regenDelay(2.f), sinceDamage(0.f), invulnTimeRemaining(0.f)
{
    shape.setSize({32.f, 32.f});
    shape.setFillColor(sf::Color::Green);
    shape.setOrigin(shape.getSize() / 2.f);
    shape.setPosition(sf::Vector2f{512.f, 384.f});
}

void Player::update(sf::Time dt) {
    vel = {0.f, 0.f};
    if (input.isKeyDown(sf::Keyboard::Key::W)) vel.y -= speed;
    if (input.isKeyDown(sf::Keyboard::Key::S)) vel.y += speed;
    if (input.isKeyDown(sf::Keyboard::Key::A)) vel.x -= speed;
    if (input.isKeyDown(sf::Keyboard::Key::D)) vel.x += speed;
    if (input.wasKeyPressed(sf::Keyboard::Key::E)) interactPressed = true;
    if (invulnTimeRemaining > 0.f) {
        invulnTimeRemaining -= dt.asSeconds();
        if (invulnTimeRemaining <= 0.f) { invulnTimeRemaining = 0.f; shape.setFillColor(sf::Color::Green); }
        else {
            // simple flicker: toggle visibility alpha
            sf::Color c = shape.getFillColor();
            if (static_cast<int>(invulnTimeRemaining * 20.f) % 2 == 0) c.a = 80; else c.a = 200;
            shape.setFillColor(c);
        }
    }
    updateHealthRegen(dt);
}

sf::Vector2f Player::computeDesiredMove(sf::Time dt) const { return vel * dt.asSeconds(); }
void Player::applyMove(const sf::Vector2f& delta) { shape.move(delta); }
void Player::setPosition(const sf::Vector2f& pos) { shape.setPosition(pos); }
sf::Vector2f Player::size() const { return shape.getSize(); }
void Player::draw(sf::RenderWindow& window) { window.draw(shape); }
sf::FloatRect Player::getBounds() const { return shape.getGlobalBounds(); }
void Player::interact(Entity* other) { if (!other) return; if (auto itemEnt = dynamic_cast<ItemEntity*>(other)) itemEnt->interact(this); }
bool Player::wantsToInteract() const { return interactPressed; }
void Player::resetInteract() { interactPressed = false; }
sf::Vector2f Player::position() const { return shape.getPosition(); }
Inventory& Player::inventory() { return inv; }
void Player::updateHealthRegen(sf::Time dt) {
    sinceDamage += dt.asSeconds();
    if (health < maxHealth && sinceDamage >= regenDelay) {
        health = std::min(maxHealth, health + regenRate * dt.asSeconds());
    }
}