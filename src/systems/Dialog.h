#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <memory>

class InputManager;

class DialogManager {
public:
    DialogManager();

    // start a dialog (lines shown in sequence)
    void start(const std::vector<std::string>& lines);

    // advance by one line (also callable from input)
    void advance();

    // update will check input to advance when active
    void update(InputManager& input, sf::Time dt);

    // draw dialog overlay
    void draw(sf::RenderWindow& window);

    bool active() const;

    // optionally provide a font from your ResourceManager
    void setFont(const std::shared_ptr<sf::Font>& f);

private:
    std::vector<std::string> lines;
    std::size_t index = 0;
    bool running = false;

    // rendering helpers
    sf::RectangleShape bg;
    // text stored via pointer because some SFML builds require construction with a Font
    std::unique_ptr<sf::Text> text;
    std::shared_ptr<sf::Font> fontFallback;
    float padding = 12.f;

    void ensureTextSetup();
};