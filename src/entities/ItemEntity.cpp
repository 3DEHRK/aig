#include "ItemEntity.h"
#include "../entities/Player.h"
#include <iostream>

ItemEntity::ItemEntity(ItemPtr item, const sf::Vector2f& pos)
: item_(std::move(item)), collected_(false)
{
    shape.setSize({20.f, 20.f});
    shape.setFillColor(sf::Color(200, 180, 20));
    shape.setOrigin(shape.getSize() / 2.f);
    shape.setPosition(pos);
}

void ItemEntity::update(sf::Time) {
    // placeholder: could animate / bob up and down
}

void ItemEntity::draw(sf::RenderWindow& window) {
    if (!collected_) window.draw(shape);
}

sf::FloatRect ItemEntity::getBounds() const { return shape.getGlobalBounds(); }

void ItemEntity::interact(Entity* other) {
    // when player interacts, transfer item to player's inventory
    if (collected_) return;
    if (!other) return;
    // attempt to find Player by dynamic_cast
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