#pragma once
#include <SFML/Window.hpp>
#include <unordered_map>
#include <vector>

class InputManager {
public:
    // sample keyboard & mouse state (call once per frame before reading input in updates)
    void poll();

    // keyboard query helpers
    bool isKeyDown(sf::Keyboard::Key k) const;
    bool wasKeyPressed(sf::Keyboard::Key k);

    // mouse query helpers
    bool isMouseDown(sf::Mouse::Button b) const;
    bool wasMousePressed(sf::Mouse::Button b);

    // clear per-frame "pressed" states (call at end of frame if needed)
    void clearFrame();

private:
    // only common keys tracked by default - extend as needed
    static const std::vector<sf::Keyboard::Key> defaultTrackedKeys;
    static const std::vector<sf::Mouse::Button> defaultTrackedMouseButtons;

    std::unordered_map<int, bool> down;    // current down state (keys & mouse stored by int)
    std::unordered_map<int, bool> pressed; // became pressed this frame
    std::unordered_map<int, bool> last;    // previous frame down state
};