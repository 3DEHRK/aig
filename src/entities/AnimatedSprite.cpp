#include "AnimatedSprite.h"
#include <type_traits>

AnimatedSprite::AnimatedSprite() = default;

void AnimatedSprite::setTexture(const std::shared_ptr<sf::Texture>& tex) {
    texture = tex;
    if (!texture) return;
    if (!sprite) {
        // construct sprite with texture (works for SFML variants requiring texture in ctor)
        sprite = std::make_unique<sf::Sprite>(*texture);
    } else {
        sprite->setTexture(*texture);
    }
}

void AnimatedSprite::addAnimation(const Animation& anim) {
    animator.addAnimation(anim);
}

void AnimatedSprite::play(const std::string& name, bool restart) {
    animator.play(name, restart);
    const Animation* a = animator.getCurrentAnimation();
    if (a && !a->frames.empty() && sprite) sprite->setTextureRect(a->frames[0]);
}

void AnimatedSprite::stop() {
    animator.stop();
}

void AnimatedSprite::update(sf::Time dt) {
    const sf::IntRect* r = animator.update(dt.asSeconds());
    if (r && sprite) sprite->setTextureRect(*r);
}

void AnimatedSprite::draw(sf::RenderWindow& window) {
    if (sprite) window.draw(*sprite);
}

sf::FloatRect AnimatedSprite::getBounds() const {
    if (sprite) return sprite->getGlobalBounds();
    return sf::FloatRect{};
}

void AnimatedSprite::setPosition(const sf::Vector2f& p) {
    if (sprite) sprite->setPosition(p);
}

sf::Vector2f AnimatedSprite::position() const {
    if (sprite) return sprite->getPosition();
    return {0.f, 0.f};
}

sf::Vector2f AnimatedSprite::size() const {
    // prefer texture size when available
    if (texture) {
        auto ts = texture->getSize();
        sf::Vector2f scale{1.f, 1.f};
        if (sprite) scale = sprite->getScale();
        return { float(ts.x) * scale.x, float(ts.y) * scale.y };
    }

    // fallback to sprite local bounds if sprite present
    if (sprite) {
        sf::FloatRect lb = sprite->getLocalBounds();
        // SFML3: use .size, SFML2: width/height — pick whichever exists
        #if defined(SFML_VERSION_MAJOR) && SFML_VERSION_MAJOR >= 3
            return { lb.size.x * sprite->getScale().x, lb.size.y * sprite->getScale().y };
        #else
            return { lb.width * sprite->getScale().x, lb.height * sprite->getScale().y };
        #endif
    }

    return {0.f, 0.f};
}