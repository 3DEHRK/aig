#include "NPC.h"
#include <iostream>

NPC::NPC(const sf::Vector2f& pos) {
    shape.setSize({32.f, 32.f});
    shape.setFillColor(sf::Color::Red);
    shape.setOrigin(shape.getSize() / 2.f);
    shape.setPosition(pos);
}

void NPC::update(sf::Time) {}

void NPC::draw(sf::RenderWindow& window) {
    window.draw(shape);
}

sf::FloatRect NPC::getBounds() const { return shape.getGlobalBounds(); }

void NPC::interact(Entity* /*by*/) {
    std::cout << "NPC: Hello!\n";
}