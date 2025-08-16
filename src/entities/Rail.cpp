#include "Rail.h"
#include "../resources/ResourceManager.h"
#include <iostream>

Rail::Rail(ResourceManager& resources, const sf::Vector2f& pos, unsigned tileSize_)
: tileSize(tileSize_)
{
    try {
        sf::Texture& t = resources.texture("assets/textures/tiles/rail.png");
        sprite = std::make_unique<sf::Sprite>(t);
        // smart scaling: fit texture into tileSize box while preserving aspect
        auto tb = sprite->getLocalBounds();
        float sx = (float)tileSize / tb.size.x;
        float sy = (float)tileSize / tb.size.y;
        float sc = std::min(sx, sy);
        // avoid huge upscales: if texture is huge relative to tile, scale down to 10% max
        const float maxDownscale = 0.10f;
        if (sc < maxDownscale) sc = maxDownscale;
        sprite->setScale(sf::Vector2f(sc, sc));
        sprite->setPosition(pos);
    } catch (...) {
        shape.setSize({(float)tileSize, (float)tileSize / 4.f});
        shape.setFillColor(sf::Color(90,90,90));
        shape.setPosition(pos);
    }
}

void Rail::update(sf::Time) { }

void Rail::draw(sf::RenderWindow& window) {
    if (sprite) window.draw(*sprite);
    else window.draw(shape);
}

sf::FloatRect Rail::getBounds() const { return sprite ? sprite->getGlobalBounds() : shape.getGlobalBounds(); }

void Rail::interact(Entity* by) { }
