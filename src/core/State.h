#pragma once
#include <SFML/Graphics.hpp>

class Game;
class State {
public:
    virtual ~State() = default;
    virtual void handleEvent(const sf::Event&) = 0;
    virtual void update(sf::Time) = 0;
    virtual void draw() = 0;
protected:
    State() = default;
};