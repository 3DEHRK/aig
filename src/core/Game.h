#pragma once

#include <SFML/Graphics.hpp>

#include <memory>

class State;
class ResourceManager;
class InputManager;
class SoundManager;

class Game {
public:
  Game();
  ~Game();
  void run();
  sf::RenderWindow& getWindow() { return window; }
  ResourceManager& resources();
  InputManager& input();
  SoundManager& sound();
  void setState(std::unique_ptr<State> s);  // allow external state change
  void pushTemporaryState(std::unique_ptr<State> s);  // save current, replace with temp
  void popTemporaryState();  // restore saved if present
  // Perform one fixed update tick (headless / testing). dtSeconds typical 1/60.f
  void step(float dtSeconds);

private:
  void processEvents();
  void update(sf::Time dt);
  void render();

  sf::RenderWindow window;
  std::unique_ptr<State> currentState;
  std::unique_ptr<State> savedState;  // holds previous PlayState during temporary realm
  std::unique_ptr<ResourceManager> resourceManager;
  std::unique_ptr<InputManager> inputManager;
  std::unique_ptr<SoundManager> soundManager;
  sf::View camera;
};