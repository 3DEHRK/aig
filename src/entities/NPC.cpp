#include "NPC.h"
#include "../resources/ResourceManager.h"
#include <SFML/Config.hpp>
#include <iostream>

static sf::Vector2f rect_size_local(const sf::FloatRect& r) {
#if defined(SFML_VERSION_MAJOR) && (SFML_VERSION_MAJOR >= 3)
    return r.size;
#else
    return {r.width, r.height};
#endif
}

NPC::NPC(ResourceManager& resources, const sf::Vector2f& pos) {
    // try to load sprite texture for NPC
    try {
        sf::Texture& tex = resources.texture("assets/textures/entities/npc_idle.png");
        sprite = std::make_unique<sf::Sprite>(tex);
        // center origin
        auto bounds = sprite->getLocalBounds();
        auto sz = rect_size_local(bounds);
        std::cerr << "NPC texture size: " << sz.x << " x " << sz.y << "\n";
        // clamp scale to reasonable size (approx 36px width)
        const float targetPixelWidth = 36.f; // increased for slightly larger NPCs
        const float maxScale = 4.f;
        const float minScale = 0.01f;
        if (sz.x > 0 && sz.y > 0) {
            float s = targetPixelWidth / sz.x;
            if (s > maxScale) s = maxScale;
            if (s < minScale) s = minScale;
            sprite->setScale(sf::Vector2f(s, s));
        }
        auto newSz = rect_size_local(sprite->getLocalBounds());
        sprite->setOrigin(sf::Vector2f(newSz.x/2.f, newSz.y/2.f));
        sprite->setPosition(pos);
        useSprite = true;
    } catch (...) {
        shape.setSize({32.f, 32.f});
        shape.setFillColor(sf::Color::Red);
        shape.setOrigin(shape.getSize() / 2.f);
        shape.setPosition(pos);
        useSprite = false;
    }
}

void NPC::update(sf::Time) {
    // simple idle - expand with AI system
}

void NPC::draw(sf::RenderWindow& window) {
    if (useSprite && sprite) window.draw(*sprite);
    else window.draw(shape);
}

sf::FloatRect NPC::getBounds() const { return useSprite && sprite ? sprite->getGlobalBounds() : shape.getGlobalBounds(); }

void NPC::interact(Entity* by) {
    // silent by default
}