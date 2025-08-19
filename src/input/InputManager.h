#pragma once
#include <SFML/Window.hpp>
#include <unordered_map>
#include <vector>
#include <string>

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

    // Action mapping -------------------------------------------------
    void bindAction(const std::string& action, sf::Keyboard::Key key) { actionToKey[action] = key; }
    sf::Keyboard::Key keyFor(const std::string& action) const {
        auto it = actionToKey.find(action); return it==actionToKey.end()? sf::Keyboard::Key::Unknown : it->second; }
    bool actionDown(const std::string& action) const {
        auto k = keyFor(action); return k!=sf::Keyboard::Key::Unknown && isKeyDown(k); }
    bool actionPressed(const std::string& action) { auto k = keyFor(action); return k!=sf::Keyboard::Key::Unknown && wasKeyPressed(k); }

    const std::unordered_map<std::string, sf::Keyboard::Key>& bindings() const { return actionToKey; }
    void setBindings(const std::unordered_map<std::string, sf::Keyboard::Key>& m) { actionToKey = m; }
private:
    // only common keys tracked by default - extend as needed
    static const std::vector<sf::Keyboard::Key> defaultTrackedKeys;
    static const std::vector<sf::Mouse::Button> defaultTrackedMouseButtons;

    std::unordered_map<int, bool> down;    // current down state (keys & mouse stored by int)
    std::unordered_map<int, bool> pressed; // became pressed this frame
    std::unordered_map<int, bool> last;    // previous frame down state
    std::unordered_map<std::string, sf::Keyboard::Key> actionToKey; // user-configurable bindings

    // Suggested default actions: MoveUp, MoveDown, MoveLeft, MoveRight, Interact, Shoot, Inventory, Help, RailTool
    // Bindings loaded from bindings.json if present.
};