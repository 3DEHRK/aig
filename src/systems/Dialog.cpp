#include "Dialog.h"
#include "../input/InputManager.h"
#include "../resources/ResourceManager.h"
#include <iostream>

DialogManager::DialogManager() {
    bg.setFillColor(sf::Color(0, 0, 0, 160));
    // create a fallback (empty) font object so we can construct sf::Text in all SFML variants.
    // You can override it later via setFont().
    fontFallback = std::make_shared<sf::Font>();
    // construct text with the fallback font (Font is the first parameter in this SFML version)
    text = std::make_unique<sf::Text>(*fontFallback, sf::String(), 18);
    text->setFillColor(sf::Color::White);
    text->setCharacterSize(18);
}

void DialogManager::setFont(const std::shared_ptr<sf::Font>& f) {
    if (!f) return;
    fontFallback = f;
    if (!text) text = std::make_unique<sf::Text>(*fontFallback, sf::String(), 18);
    else text->setFont(*fontFallback);
}

void DialogManager::ensureTextSetup() {
    if (!text) {
        text = std::make_unique<sf::Text>(*fontFallback, sf::String(), 18);
        text->setFillColor(sf::Color::White);
        text->setCharacterSize(18);
    }
}

void DialogManager::start(const std::vector<std::string>& l) {
    lines = l;
    index = 0;
    running = !lines.empty();
}

void DialogManager::advance() {
    if (!running) return;
    ++index;
    if (index >= lines.size()) {
        running = false;
        lines.clear();
        index = 0;
    }
}

void DialogManager::update(InputManager& input, sf::Time /*dt*/) {
    if (!running) return;
    // advance on E or Space (consume the press)
    if (input.wasKeyPressed(sf::Keyboard::Key::E) || input.wasKeyPressed(sf::Keyboard::Key::Space)) {
        advance();
    }
}

void DialogManager::draw(sf::RenderWindow& window) {
    if (!running) return;
    ensureTextSetup();

    const sf::Vector2u winSz = window.getSize();
    const float w = float(winSz.x);
    const float h = float(winSz.y) * 0.28f;
    bg.setSize(sf::Vector2f{w - 40.f, h});
    bg.setPosition(sf::Vector2f{20.f, float(winSz.y) - h - 20.f});

    // prepare text
    std::string s = lines[index];
    text->setString(s);
    // position text inside bg
    text->setPosition(bg.getPosition() + sf::Vector2f(padding, padding));
    window.draw(bg);
    window.draw(*text);
}

bool DialogManager::active() const {
    return running;
}