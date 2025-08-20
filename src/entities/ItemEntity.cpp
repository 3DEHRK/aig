#include "ItemEntity.h"
#include "../entities/Player.h"
#include <iostream>
#include <cmath>

ItemEntity::ItemEntity(ItemPtr item, const sf::Vector2f& pos)
: item_(std::move(item)), collected_(false)
{
    shape.setSize({20.f, 20.f});
    shape.setFillColor(sf::Color(200, 180, 20));
    shape.setOrigin(shape.getSize() / 2.f);
    shape.setPosition(pos);
}

void ItemEntity::startMagnet() { magnetizing = true; }

void ItemEntity::update(sf::Time dt) {
    if (collected_) return;
    if (magnetizing) {
        float ds = dt.asSeconds();
        // simple homing accel toward player (player assumed global singleton accessible via interaction loop; here we just adjust in collect phase)
        // velocity already pointing last frame; small damping
        velocity *= 0.92f;
        // will re-target in PlayState update (we don't know player pos here without passing pointer) -> keep placeholder drift
        shape.move(velocity * ds);
    }
}

void ItemEntity::draw(sf::RenderWindow& window) {
    if (!collected_) window.draw(shape);
}

sf::FloatRect ItemEntity::getBounds() const { return shape.getGlobalBounds(); }

void ItemEntity::interact(Entity* other) {
    if (collected_) return;
    if (!other) return;
    if (auto p = dynamic_cast<Player*>(other)) {
        if (p->inventory().addItem(item_)) {
            collected_ = true;
            std::cout << "Picked up: " << (item_ ? item_->name : "unknown") << "\n";
        } else {
            std::cout << "Inventory full, cannot pick up: " << (item_ ? item_->name : "unknown") << "\n";
        }
    }
}

bool ItemEntity::collected() const { return collected_; }
ItemPtr ItemEntity::item() const { return item_; }
void ItemEntity::collect() { collected_ = true; }