#include "InputManager.h"
#include <iostream>

const std::vector<sf::Keyboard::Key> InputManager::defaultTrackedKeys = {
    sf::Keyboard::Key::W,
    sf::Keyboard::Key::A,
    sf::Keyboard::Key::S,
    sf::Keyboard::Key::D,
    sf::Keyboard::Key::E,
    sf::Keyboard::Key::Space,
    sf::Keyboard::Key::LShift,
    sf::Keyboard::Key::Escape,
    sf::Keyboard::Key::Left,
    sf::Keyboard::Key::Right,
    sf::Keyboard::Key::Up,
    sf::Keyboard::Key::Down,
    // additional keys used by the UI and tools
    sf::Keyboard::Key::I,    // inventory toggle
    sf::Keyboard::Key::B,    // rail build tool toggle
    sf::Keyboard::Key::F5,   // save
    sf::Keyboard::Key::F9    // load
};

// track common mouse buttons
const std::vector<sf::Mouse::Button> InputManager::defaultTrackedMouseButtons = {
    sf::Mouse::Button::Left,
    sf::Mouse::Button::Right,
    sf::Mouse::Button::Middle
};

void InputManager::poll() {
    // sample tracked keys and compute transitions
    for (auto k : defaultTrackedKeys) {
        bool cur = sf::Keyboard::isKeyPressed(k);
        int idx = static_cast<int>(k);
        bool prev = false;
        auto it = last.find(idx);
        if (it != last.end()) prev = it->second;

        down[idx] = cur;
        pressed[idx] = (cur && !prev);
        last[idx] = cur;

        // debug: log specific key presses to help diagnose missed inputs
        if (pressed[idx]) {
            if (k == sf::Keyboard::Key::E) std::cerr << "[InputManager] E pressed\n";
            else if (k == sf::Keyboard::Key::I) std::cerr << "[InputManager] I pressed\n";
            else if (k == sf::Keyboard::Key::B) std::cerr << "[InputManager] B pressed\n";
            else if (k == sf::Keyboard::Key::Space) std::cerr << "[InputManager] Space pressed\n";
        }
    }

    // sample mouse buttons
    for (auto b : defaultTrackedMouseButtons) {
        bool cur = sf::Mouse::isButtonPressed(b);
        int idx = static_cast<int>(b) + 1000; // offset to avoid clashing with key indices
        bool prev = false;
        auto it = last.find(idx);
        if (it != last.end()) prev = it->second;

        down[idx] = cur;
        pressed[idx] = (cur && !prev);
        last[idx] = cur;
    }
}

bool InputManager::isKeyDown(sf::Keyboard::Key k) const {
    auto it = down.find(static_cast<int>(k));
    return it != down.end() && it->second;
}

bool InputManager::wasKeyPressed(sf::Keyboard::Key k) {
    auto it = pressed.find(static_cast<int>(k));
    if (it != pressed.end() && it->second) {
        it->second = false;
        return true;
    }
    return false;
}

bool InputManager::isMouseDown(sf::Mouse::Button b) const {
    auto it = down.find(static_cast<int>(b) + 1000);
    return it != down.end() && it->second;
}

bool InputManager::wasMousePressed(sf::Mouse::Button b) {
    auto it = pressed.find(static_cast<int>(b) + 1000);
    if (it != pressed.end() && it->second) {
        it->second = false;
        return true;
    }
    return false;
}

void InputManager::clearFrame() {
    pressed.clear();
}