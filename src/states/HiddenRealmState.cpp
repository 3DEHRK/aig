#include "HiddenRealmState.h"
#include "../core/Game.h"
#include "../states/PlayState.h"
#include <iostream>

HiddenRealmState::HiddenRealmState(Game& g)
: game(g)
{
    timer = 0.f;
    std::cerr << "Entered Hidden Realm (stub)." << std::endl;
}

void HiddenRealmState::handleEvent(const sf::Event&) {}

void HiddenRealmState::update(sf::Time dt) {
    timer += dt.asSeconds();
    // automatic exit after 30 seconds for now
    if (timer > 30.f) {
        std::cerr << "Exiting Hidden Realm after timeout, returning to PlayState\n";
        game.setState(std::make_unique<PlayState>(game));
        return;
    }
}

void HiddenRealmState::draw() {
    auto& win = game.getWindow();
    win.clear(sf::Color(20, 10, 30));
    // placeholder: draw a pulsing circle in the center
    sf::CircleShape c(80.f);
    c.setFillColor(sf::Color(120, 80, 200));
    c.setOrigin({80.f, 80.f});
    c.setPosition(win.getView().getCenter());
    win.draw(c);
}
