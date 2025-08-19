#include "InputManager.h"

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
    // added keys for action bindings
    sf::Keyboard::Key::I,
    sf::Keyboard::Key::M,
    sf::Keyboard::Key::N,
    sf::Keyboard::Key::T,
    sf::Keyboard::Key::U,
    sf::Keyboard::Key::J,
    sf::Keyboard::Key::V,
    sf::Keyboard::Key::G,
    sf::Keyboard::Key::Y,
    sf::Keyboard::Key::K,
    sf::Keyboard::Key::L,
    sf::Keyboard::Key::P,
    sf::Keyboard::Key::O,
    sf::Keyboard::Key::C,
    sf::Keyboard::Key::F,
    sf::Keyboard::Key::B,
    sf::Keyboard::Key::H
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