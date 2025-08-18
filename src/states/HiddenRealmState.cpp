#include "HiddenRealmState.h"
#include "../core/Game.h"
#include <SFML/Graphics.hpp>
#include <iostream>

HiddenRealmState::HiddenRealmState(Game& g)
: game(g), timer(0.f) {
    std::cerr << "Entered Hidden Realm (stub)." << std::endl;
}

void HiddenRealmState::handleEvent(const sf::Event&) {}

void HiddenRealmState::update(sf::Time dt) {
    timer += dt.asSeconds();
    if (timer > 30.f) {
        std::cerr << "Leaving Hidden Realm after timer.\n";
        game.popTemporaryState();
    }
}

void HiddenRealmState::draw() {
    auto& win = game.getWindow();
    win.clear(sf::Color(20, 10, 30));
    sf::CircleShape c(80.f);
    c.setFillColor(sf::Color(120, 80, 200));
    c.setOrigin({80.f, 80.f});
    c.setPosition(win.getView().getCenter());
    win.draw(c);
}
