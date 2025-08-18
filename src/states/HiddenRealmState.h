#pragma once
#include "../core/State.h"
#include <SFML/Graphics.hpp>

class Game;

class HiddenRealmState : public State {
public:
    HiddenRealmState(Game& g);
    void handleEvent(const sf::Event&) override;
    void update(sf::Time) override;
    void draw() override;
private:
    Game& game;
    float timer = 0.f;
};