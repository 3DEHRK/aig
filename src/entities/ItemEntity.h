#pragma once
#include "Entity.h"
#include "../items/Item.h"
#include <SFML/Graphics.hpp>
#include <memory>

class ItemEntity : public Entity {
public:
    ItemEntity(ItemPtr item, const sf::Vector2f& pos);
    void update(sf::Time) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
    void interact(Entity* other) override;
    bool collected() const;
    ItemPtr item() const;
    void collect();
private:
    ItemPtr item_;
    sf::RectangleShape shape;
    bool collected_ = false;
};