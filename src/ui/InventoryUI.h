#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

#include "../items/Item.h"

class ResourceManager;
class Inventory;
class InputManager;

class InventoryUI {
public:
    InventoryUI(ResourceManager& resources, Inventory& inventory);
    void toggle();
    bool isOpen() const { return open; }
    // updated to take the render window so UI can read mouse coords
    void update(InputManager& input, sf::RenderWindow& window, sf::Time dt);
    void draw(sf::RenderWindow& window);
private:
    ResourceManager& resources;
    Inventory& inventory;
    bool open = false;
    sf::RectangleShape bg;
    sf::Font* font = nullptr;
    std::unique_ptr<sf::Text> title;

    // grid layout
    unsigned cols = 8;
    unsigned rows = 0;
    float slotSize = 48.f;
    float padding = 8.f;

    // drag & drop
    ItemPtr draggingItem;
    int dragFromIndex = -1;
    sf::Vector2f dragOffset;
};
