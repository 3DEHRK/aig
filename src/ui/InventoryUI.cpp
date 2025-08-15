#include "InventoryUI.h"
#include "../resources/ResourceManager.h"
#include "../systems/Inventory.h"
#include "../input/InputManager.h"
#include <iostream>

InventoryUI::InventoryUI(ResourceManager& resources_, Inventory& inventory_)
: resources(resources_), inventory(inventory_)
{
    bg.setSize({600.f, 340.f});
    bg.setFillColor(sf::Color(20, 20, 30, 220));
    bg.setOutlineColor(sf::Color::White);
    bg.setOutlineThickness(2.f);
    bg.setPosition(sf::Vector2f(20.f, 20.f));

    rows = static_cast<unsigned>((inventory.capacity() + cols - 1) / cols);

    try {
        auto &f = resources.font("assets/fonts/arial.ttf");
        font = &f;
        title = std::make_unique<sf::Text>(*font, "Inventory", 18);
        title->setFillColor(sf::Color::White);
        title->setPosition(sf::Vector2f(30.f, 24.f));
    } catch (...) {
        // silent
    }
}

void InventoryUI::toggle() { open = !open; }

void InventoryUI::update(InputManager& input, sf::RenderWindow& window, sf::Time) {
    if (!open) return;
    if (input.wasKeyPressed(sf::Keyboard::Key::I)) open = false;

    // basic mouse drag/drop: left click pick up, click another slot to swap/place
    sf::Vector2i mpos_i = sf::Mouse::getPosition(window);
    sf::Vector2f mf((float)mpos_i.x, (float)mpos_i.y);

    // compute slot rectangles
    float startX = bg.getPosition().x + 20.f;
    float startY = bg.getPosition().y + 60.f;

    for (unsigned r=0; r<rows; ++r) {
        for (unsigned c=0; c<cols; ++c) {
            unsigned idx = r*cols + c;
            if (idx >= inventory.capacity()) continue;
            sf::FloatRect slotRect(sf::Vector2f(startX + c*(slotSize+padding), startY + r*(slotSize+padding)), sf::Vector2f(slotSize, slotSize));
            bool hover = slotRect.contains(mf);
            if (hover && input.wasMousePressed(sf::Mouse::Button::Left)) {
                if (!draggingItem) {
                    // pick up
                    draggingItem = inventory.getItem(idx);
                    if (draggingItem) {
                        inventory.setItem(idx, nullptr);
                        dragFromIndex = idx;
                        dragOffset = mf - sf::Vector2f(slotRect.position.x, slotRect.position.y);
                    }
                } else {
                    // place / swap
                    ItemPtr current = inventory.getItem(idx);
                    inventory.setItem(idx, draggingItem);
                    if (dragFromIndex >= 0) inventory.setItem(dragFromIndex, current);
                    draggingItem = nullptr;
                    dragFromIndex = -1;
                }
            }
        }
    }
}

void InventoryUI::draw(sf::RenderWindow& window) {
    if (!open) return;
    window.draw(bg);
    if (title) window.draw(*title);

    float startX = bg.getPosition().x + 20.f;
    float startY = bg.getPosition().y + 60.f;

    // draw slots
    for (unsigned r=0; r<rows; ++r) {
        for (unsigned c=0; c<cols; ++c) {
            unsigned idx = r*cols + c;
            if (idx >= inventory.capacity()) continue;
            sf::RectangleShape slotBg(sf::Vector2f(slotSize, slotSize));
            slotBg.setPosition(sf::Vector2f(startX + c*(slotSize+padding), startY + r*(slotSize+padding)));
            slotBg.setFillColor(sf::Color(40,40,50));
            slotBg.setOutlineColor(sf::Color::White);
            slotBg.setOutlineThickness(1.f);
            window.draw(slotBg);

            auto it = inventory.getItem(idx);
            if (it) {
                if (!it->iconPath.empty()) {
                    try {
                        sf::Texture& t = resources.texture(it->iconPath);
                        sf::Sprite s(t);
                        // fit sprite inside slot
                        auto tb = s.getLocalBounds();
                        float sx = slotSize / tb.size.x;
                        float sy = slotSize / tb.size.y;
                        float sc = std::min(sx, sy);
                        s.setScale(sf::Vector2f(sc, sc));
                        s.setPosition(slotBg.getPosition());
                        window.draw(s);
                    } catch (...) {
                        // fallback to text
                        sf::Text t(*font, it->name, 12);
                        t.setFillColor(sf::Color::White);
                        t.setPosition(sf::Vector2f(slotBg.getPosition().x + 4.f, slotBg.getPosition().y + 4.f));
                        window.draw(t);
                    }
                } else {
                    sf::Text t(*font, it->name, 12);
                    t.setFillColor(sf::Color::White);
                    t.setPosition(sf::Vector2f(slotBg.getPosition().x + 4.f, slotBg.getPosition().y + 4.f));
                    window.draw(t);
                }

                // draw stack size if >1
                if (it->stackSize > 1) {
                    sf::Text s(*font, std::to_string(it->stackSize), 10);
                    s.setFillColor(sf::Color::Yellow);
                    s.setPosition(sf::Vector2f(slotBg.getPosition().x + slotSize - 16.f, slotBg.getPosition().y + slotSize - 16.f));
                    window.draw(s);
                }
            }
        }
    }

    // draw dragging item following mouse
    if (draggingItem && font) {
        sf::Vector2i mpos_i = sf::Mouse::getPosition(window);
        sf::Text t(*font, draggingItem->name, 12);
        t.setFillColor(sf::Color::White);
        t.setPosition(sf::Vector2f((float)mpos_i.x - dragOffset.x, (float)mpos_i.y - dragOffset.y));
        window.draw(t);
    }
}
