#include "Rail.h"
#include "../resources/ResourceManager.h"
#include <iostream>

Rail::Rail(ResourceManager& resources, const sf::Vector2f& pos) {
    try {
        sf::Texture& t = resources.texture("assets/textures/tiles/rail.png");
        sprite = std::make_unique<sf::Sprite>(t);
        sprite->setPosition(pos);
        useSprite = true;
    } catch (...) {
        shape.setSize({32.f, 8.f});
        shape.setFillColor(sf::Color(90,90,90));
        shape.setPosition(pos);
        useSprite = false;
    }
}

void Rail::update(sf::Time) { }

void Rail::draw(sf::RenderWindow& window) {
    if (useSprite && sprite) window.draw(*sprite);
    else window.draw(shape);
}

sf::FloatRect Rail::getBounds() const { return useSprite && sprite ? sprite->getGlobalBounds() : shape.getGlobalBounds(); }

void Rail::interact(Entity* by) { }
