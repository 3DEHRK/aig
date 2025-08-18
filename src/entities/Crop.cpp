#include "Crop.h"
#include "Player.h"
#include "../resources/ResourceManager.h"
#include <iostream>

static sf::Vector2f rect_size_local(const sf::FloatRect& r) {
#if defined(SFML_VERSION_MAJOR) && (SFML_VERSION_MAJOR >= 3)
    return r.size;
#else
    return {r.width, r.height};
#endif
}

Crop::Crop(ResourceManager& resources, const sf::Vector2f& pos, const std::string& seedId, int stages, float secondsPerStage)
: stage(0), maxStage(stages), timer(0.f), secondsPerStage(secondsPerStage), harvested(false)
{
    // attempt to load stage textures named crop1.png, crop2.png, ...
    for (int i = 1; i <= maxStage; ++i) {
        std::string p = "assets/textures/entities/crop" + std::to_string(i) + ".png";
        try {
            sf::Texture& t = resources.texture(p);
            stageTextures.push_back(&t);
        } catch (...) {
            // missing stage texture: stop collecting further textures
            break;
        }
    }

    if (!stageTextures.empty()) {
        sprite = std::make_unique<sf::Sprite>(*stageTextures[0]);
        // scale crops to 1/7 of original size
        const float cropScale = 1.0f / 7.0f;
        sprite->setScale(sf::Vector2f(cropScale, cropScale));
        // set origin before positioning so centering works predictably
        auto loc = sprite->getLocalBounds();
        auto sz = rect_size_local(loc);
        sprite->setOrigin(sf::Vector2f(sz.x/2.f, sz.y/2.f));
        sprite->setPosition(pos);
        useSprite = true;
        std::cerr << "Crop created with " << stageTextures.size() << " stage textures\n";
    } else {
        shape.setSize({24.f, 24.f});
        shape.setOrigin(shape.getSize() / 2.f);
        shape.setPosition(pos);
        // ensure fallback shape is visible immediately
        shape.setFillColor(sf::Color(200, 160, 60));
        shape.setOutlineColor(sf::Color::Black);
        shape.setOutlineThickness(1.f);
        useSprite = false;
    }

    // simple yield item based on seed id
    yieldItem = std::make_shared<Item>("crop_" + seedId, "Crop", "A harvested crop", 1);
}

void Crop::update(sf::Time dt) {
    if (harvested) return;
    timer += dt.asSeconds();
    if (timer >= secondsPerStage) {
        timer = 0.f;
        ++stage;
        if (stage >= maxStage) stage = maxStage;
        if (!useSprite) {
            float t = float(stage) / float(maxStage);
            shape.setFillColor(sf::Color(100 + int(155 * t), 180, 80));
        } else {
            // update sprite texture pointer if available
            int idx = std::min(stage, (int)stageTextures.size() - 1);
            if (idx >= 0 && idx < (int)stageTextures.size() && stageTextures[idx]) {
                // preserve current position while changing texture/origin
                sf::Vector2f curPos = sprite->getPosition();
                sprite->setTexture(*stageTextures[idx]);
                const float cropScale = 1.0f / 7.0f;
                sprite->setScale(sf::Vector2f(cropScale, cropScale));
                auto loc = sprite->getLocalBounds();
                auto sz = rect_size_local(loc);
                sprite->setOrigin(sf::Vector2f(sz.x/2.f, sz.y/2.f));
                sprite->setPosition(curPos);
            }
        }
    }
}

void Crop::draw(sf::RenderWindow& window) {
    if (harvested) return;
    if (useSprite && sprite) window.draw(*sprite);
    else window.draw(shape);
}

sf::FloatRect Crop::getBounds() const { return useSprite && sprite ? sprite->getGlobalBounds() : shape.getGlobalBounds(); }

void Crop::interact(Entity* other) {
    if (harvested) return;
    if (stage < maxStage) return; // not ready
    if (auto p = dynamic_cast<Player*>(other)) {
        if (p->inventory().addItem(yieldItem)) {
            harvested = true;
            std::cout << "Harvested crop and added to inventory\n";
            // TODO: play harvest sound and spawn particles (SFX manager integration pending)
            std::cerr << "(SFX) play harvest sound and spawn particles\n";
        } else {
            std::cout << "Inventory full, cannot harvest\n";
        }
    }
}
