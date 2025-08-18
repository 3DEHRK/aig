#include "Altar.h"
#include "Player.h"
#include "../resources/ResourceManager.h"
#include "../systems/Inventory.h"
#include <iostream>

Altar::Altar(ResourceManager& resources, const sf::Vector2f& pos) {
    try {
        auto &tex = resources.texture("assets/textures/entities/altar.png");
        sprite = std::make_unique<sf::Sprite>(tex);
        auto bounds = sprite->getLocalBounds();
#if defined(SFML_VERSION_MAJOR) && (SFML_VERSION_MAJOR >= 3)
        sprite->setOrigin({bounds.size.x * 0.5f, bounds.size.y * 0.5f});
#else
        sprite->setOrigin(bounds.width * 0.5f, bounds.height * 0.5f);
#endif
        sprite->setPosition(pos);
    } catch (const std::exception& e) {
        std::cerr << "Failed to load altar texture: " << e.what() << "\n";
    }
}

void Altar::update(sf::Time) {
    // no idle animation yet
}

void Altar::draw(sf::RenderWindow& win) {
    if (sprite) win.draw(*sprite);
}

sf::FloatRect Altar::getBounds() const {
    if (sprite) return sprite->getGlobalBounds();
    return sf::FloatRect();
}

void Altar::setRequiredItems(const std::vector<std::string>& items) {
    requiredItems = items;
}

void Altar::interact(Entity* by) {
    if (active) {
        std::cerr << "Altar already active." << "\n";
        return; // already activated
    }
    auto player = dynamic_cast<Player*>(by);
    if (!player) return;
    // check inventory for required items
    bool hasAll = true;
    for (auto &id : requiredItems) {
        bool found = false;
        for (auto &it : player->inventory().items()) {
            if (it && it->id == id) { found = true; break; }
        }
        if (!found) { hasAll = false; break; }
    }
    if (!hasAll) {
        std::cerr << "Altar activation failed: missing required items." << "\n";
        return;
    }
    // consume required items (simple remove one each)
    for (auto &id : requiredItems) {
        player->inventory().removeItemById(id, 1);
    }
    active = true;
    std::cerr << "Altar activated! Portal opens. This spot can now be a respawn." << "\n";
}