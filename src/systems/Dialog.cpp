#include "Dialog.h"
#include "../input/InputManager.h"
#include "../resources/ResourceManager.h"
#include <iostream>
#include <nlohmann/json.hpp>

DialogManager::DialogManager() {
    bg.setFillColor(sf::Color(0, 0, 0, 160));
    fontFallback = std::make_shared<sf::Font>();
    // Correct SFML3 constructor order: (font, string, size)
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

nlohmann::json DialogManager::toJson() const {
    nlohmann::json j;
    j["active"] = running;
    if (running) {
        // capture remaining lines starting at current index
        std::vector<std::string> remaining;
        for (size_t i = index; i < lines.size(); ++i) remaining.push_back(lines[i]);
        j["lines"] = remaining;
    }
    return j;
}

void DialogManager::fromJson(const nlohmann::json& j) {
    running = false; lines.clear(); index = 0;
    if (!j.is_object()) return;
    if (j.contains("active") && j["active"].get<bool>() && j.contains("lines") && j["lines"].is_array()) {
        lines = j["lines"].get<std::vector<std::string>>();
        running = !lines.empty();
        index = 0; // resume at first of remaining lines
    }
}