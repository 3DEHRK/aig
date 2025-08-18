#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "../systems/Inventory.h"
class ResourceManager;

class InventoryUI {
public:
    InventoryUI(ResourceManager& resources, Inventory& inv);
    void update(class InputManager& input, sf::RenderWindow& win, sf::Time dt);
    void draw(sf::RenderWindow& win);
    void toggle() { visible = !visible; }
private:
    Inventory& inventory;
    bool visible = false;
    sf::Font* font = nullptr; // non-owning
};
