#include "Rail.h"
#include "../resources/ResourceManager.h"

Rail::Rail(ResourceManager& /*resources*/, const sf::Vector2f& pos, unsigned tileSize) {
    shape.setSize({(float)tileSize, (float)tileSize});
    shape.setFillColor(sf::Color(100,80,40));
    shape.setPosition(pos);
}

void Rail::draw(sf::RenderWindow& win) { win.draw(shape); }

sf::FloatRect Rail::getBounds() const { return shape.getGlobalBounds(); }
