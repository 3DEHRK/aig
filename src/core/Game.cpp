#include "Game.h"
#include "State.h"
#include "../states/PlayState.h"
#include "../resources/ResourceManager.h"
#include "../input/InputManager.h"
#include <variant>
#include <type_traits>
#include <SFML/Config.hpp>

// helper to build an overloaded lambda for visitors
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

Game::Game()
: window(sf::VideoMode({1024u, 768u}), "Top-down Game Framework")
, camera(window.getDefaultView())
{
    resourceManager = std::make_unique<ResourceManager>();
    inputManager = std::make_unique<InputManager>();
    currentState = std::make_unique<PlayState>(*this);
    window.setFramerateLimit(60);
}

Game::~Game() = default;

// detection: does sf::Event have a `.type` member (SFML2)?
template<typename T, typename = void>
struct has_event_type_member : std::false_type {};
template<typename T>
struct has_event_type_member<T, std::void_t<decltype(std::declval<const T&>().type)>> : std::true_type {};

void Game::run() {
    sf::Clock clock;
    sf::Time accumulator = sf::Time::Zero;
    const sf::Time timestep = sf::seconds(1.f / 60.f);

    while (window.isOpen()) {
        processEvents();
        accumulator += clock.restart();
        while (accumulator >= timestep) {
            update(timestep);
            accumulator -= timestep;
        }
        render();
    }
}

void Game::processEvents() {
    auto optEvent = window.pollEvent();
    while (optEvent.has_value()) {
        const sf::Event& event = *optEvent;

        // Handle window close for SFML2 which exposes event.type and sf::Event::Closed.
#if defined(SFML_VERSION_MAJOR) && (SFML_VERSION_MAJOR < 3)
        if (event.type == sf::Event::Closed) {
            window.close();
            return;
        }
#endif

        // For SFML3, the header layout differs; fall back to Escape key handling in update().

        // Forward events to current state (InputManager is sampled per-frame in update).
        if (currentState) currentState->handleEvent(event);

        optEvent = window.pollEvent();
    }
}

void Game::update(sf::Time dt) {
    // sample current keyboard state so entities can query input during update
    inputManager->poll();

    // Allow Escape to close the window (safe, immediate shutdown)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
        window.close();
        return;
    }

    if (currentState) currentState->update(dt);
}

void Game::render() {
    window.clear(sf::Color(50, 50, 70));
    if (currentState) currentState->draw();
    window.display();
}

ResourceManager& Game::resources() { return *resourceManager; }
InputManager& Game::input() { return *inputManager; }