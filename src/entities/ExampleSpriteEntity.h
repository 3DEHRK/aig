#pragma once
#include "../entities/Entity.h"
#include <SFML/Graphics.hpp>
#include <string>

class Game;

class ExampleSpriteEntity : public Entity {
public:
    ExampleSpriteEntity(Game& game, const std::string& texturePath, const sf::Vector2f& pos);
    void update(sf::Time dt) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
    void interact(Entity* other) override;

private:
    Game& game;
    sf::Sprite sprite;
};
