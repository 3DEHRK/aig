#pragma once
#include "Entity.h"
#include "../components/Animation.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

class AnimatedSprite : public Entity {
public:
    AnimatedSprite();
    void setTexture(const std::shared_ptr<sf::Texture>& tex);
    void addAnimation(const Animation& anim);
    void play(const std::string& name, bool restart = false);
    void stop();
    void update(sf::Time dt) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
    void setPosition(const sf::Vector2f& p);
    sf::Vector2f position() const;
    sf::Vector2f size() const; // size in pixels (texture rect size or sprite scale)

private:
    std::shared_ptr<sf::Texture> texture;
    // store sprite as pointer because some SFML builds require constructing with a texture
    std::unique_ptr<sf::Sprite> sprite;
    Animator animator;
};