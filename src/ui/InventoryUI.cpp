#include "InventoryUI.h"
#include "../resources/ResourceManager.h"
#include "../input/InputManager.h"
#include <iostream>

InventoryUI::InventoryUI(ResourceManager& resources, Inventory& inv)
: inventory(inv) {
    try { font = &resources.font("assets/fonts/arial.ttf"); } catch(...) { font = nullptr; }
}

void InventoryUI::update(InputManager& /*input*/, sf::RenderWindow& /*win*/, sf::Time /*dt*/) {
    // future: hover tooltips, selection
}

void InventoryUI::draw(sf::RenderWindow& win) {
    if (!visible) return;
    sf::View prev = win.getView();
    win.setView(win.getDefaultView());
    float margin = 10.f;
    float slot = 32.f;
    float pad = 4.f;
    int cols = 8;
    auto &items = inventory.items();
    for (size_t i=0;i<items.size();++i) {
        int row = (int)i / cols;
        int col = (int)i % cols;
        sf::RectangleShape r({slot,slot});
        r.setPosition({margin + col*(slot+pad), margin + row*(slot+pad) + 60.f});
        r.setFillColor(sf::Color(30,30,30,200));
        r.setOutlineColor(sf::Color::White);
        r.setOutlineThickness(1.f);
        win.draw(r);
    }
    win.setView(prev);
}
