#include "Player.h"
#include "../input/InputManager.h"
#include "../resources/ResourceManager.h"
#include "ItemEntity.h"
#include "Projectile.h"
#include <SFML/Config.hpp>
#include <iostream>

// helper: get size of FloatRect in a SFML-version-safe way
static sf::Vector2f rect_size(const sf::FloatRect& r) {
#if defined(SFML_VERSION_MAJOR) && (SFML_VERSION_MAJOR >= 3)
    return r.size;
#else
    return {r.width, r.height};
#endif
}

Player::Player(InputManager& inputMgr, ResourceManager& resources)
: sprite(resources.texture("assets/textures/entities/player_idle.png"))
, speed(200.f), input(inputMgr), inv(32), health(100.f), maxHealth(100.f)
{
    // scale sprite to a sensible pixel size (approx 36px width), but clamp scales to avoid huge textures
    const float targetPixelWidth = 36.f; // increased for slightly larger player
    const float maxScale = 4.f;    // don't upscale more than this
    const float minScale = 0.01f;  // don't shrink below this
    auto bounds = sprite.getLocalBounds();
    auto sz = rect_size(bounds);
    if (sz.x > 0 && sz.y > 0) {
        float sx = targetPixelWidth / sz.x;
        float sy = sx; // maintain aspect
        // clamp
        if (sx > maxScale) sx = maxScale;
        if (sy > maxScale) sy = maxScale;
        if (sx < minScale) sx = minScale;
        if (sy < minScale) sy = minScale;
        sprite.setScale(sf::Vector2f(sx, sy));
    }
    sf::Vector2f origin((sz.x * sprite.getScale().x) / 2.f, (sz.y * sprite.getScale().y) / 2.f);
    sprite.setOrigin(origin);
    sprite.setPosition(sf::Vector2f{512.f, 384.f});
}

void Player::update(sf::Time /*dt*/) {
    // only update input-derived flags/velocity here (no movement commit)
    vel = {0.f, 0.f};
    if (input.isKeyDown(sf::Keyboard::Key::W)) vel.y -= speed;
    if (input.isKeyDown(sf::Keyboard::Key::S)) vel.y += speed;
    if (input.isKeyDown(sf::Keyboard::Key::A)) vel.x -= speed;
    if (input.isKeyDown(sf::Keyboard::Key::D)) vel.x += speed;
    // NOTE: do not consume the E key here (wasKeyPressed) — PlayState handles interaction presses.
}

sf::Vector2f Player::computeDesiredMove(sf::Time dt) const {
    return vel * dt.asSeconds();
}

void Player::applyMove(const sf::Vector2f& delta) {
    sprite.move(delta);
}

void Player::setPosition(const sf::Vector2f& pos) {
    sprite.setPosition(pos);
}

sf::Vector2f Player::size() const {
    auto b = sprite.getGlobalBounds();
    return rect_size(b);
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

sf::FloatRect Player::getBounds() const { return sprite.getGlobalBounds(); }

void Player::interact(Entity* other) {
    if (!other) return;
    if (auto itemEnt = dynamic_cast<ItemEntity*>(other)) {
        itemEnt->interact(this);
    }
}

void Player::fireProjectile(const sf::Vector2f& dir) {
    if (dir.x == 0 && dir.y == 0) return;
    float len = std::sqrt(dir.x*dir.x + dir.y*dir.y);
    sf::Vector2f nd = {dir.x/len, dir.y/len};
    sf::Vector2f pos = sprite.getPosition();
    sf::Vector2f vel = nd * 300.f; // projectile speed
    projectiles.push_back(std::make_unique<Projectile>(pos, vel));
}

void Player::takeDamage(float amount) {
    health -= amount;
    if (health < 0.f) health = 0.f;
    std::cerr << "Player took " << amount << " damage. health=" << health << "/" << maxHealth << "\n";
}

bool Player::wantsToInteract() const { return interactPressed; }
void Player::resetInteract() { interactPressed = false; }
sf::Vector2f Player::position() const { return sprite.getPosition(); }
Inventory& Player::inventory() { return inv; }