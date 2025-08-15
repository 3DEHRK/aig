#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

class State;
class ResourceManager;
class InputManager;

class Game {
public:
    Game();
    ~Game();
    void run();
    sf::RenderWindow& getWindow() { return window; }
    ResourceManager& resources();
    InputManager& input();
private:
    void processEvents();
    void update(sf::Time dt);
    void render();

    sf::RenderWindow window;
    std::unique_ptr<State> currentState;
    std::unique_ptr<ResourceManager> resourceManager;
    std::unique_ptr<InputManager> inputManager;
    sf::View camera;
};