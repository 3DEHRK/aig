#include "ExampleSpriteEntity.h"
#include "../core/Game.h"
#include "../resources/ResourceManager.h"

ExampleSpriteEntity::ExampleSpriteEntity(Game& g, const std::string& texturePath, const sf::Vector2f& pos)
: game(g), sprite(game.resources().texture(texturePath))
{
    sprite.setPosition(pos);
    // center origin (handle SFML2 vs SFML3 FloatRect layout)
    auto bounds = sprite.getLocalBounds();
#if defined(SFML_VERSION_MAJOR) && (SFML_VERSION_MAJOR >= 3)
    sf::Vector2f origin(bounds.size.x / 2.f, bounds.size.y / 2.f);
#else
    sf::Vector2f origin(bounds.width / 2.f, bounds.height / 2.f);
#endif
    sprite.setOrigin(origin);
}

void ExampleSpriteEntity::update(sf::Time) {
    // no-op for now
}

void ExampleSpriteEntity::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

sf::FloatRect ExampleSpriteEntity::getBounds() const { return sprite.getGlobalBounds(); }

void ExampleSpriteEntity::interact(Entity* other) {
    // example behavior: print to stderr when player interacts
    // keep silent in normal run
}
