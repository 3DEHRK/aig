#include "Game.h"
#include "State.h"
#include "../states/PlayState.h"
#include "../resources/ResourceManager.h"
#include "../input/InputManager.h"
#include "../systems/SoundManager.h"
#include <variant>
#include <type_traits>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream> // added for std::cerr

// helper to build an overloaded lambda for visitors
template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

static void loadBindings(InputManager& input) {
    std::ifstream is("bindings.json");
    if (!is) return; // optional
    try {
        nlohmann::json j; is >> j; if (!j.is_object()) return;
        if (j.contains("keys") && j["keys"].is_object()) {
            for (auto &kv : j["keys"].items()) {
                const std::string action = kv.key();
                int code = kv.value().get<int>();
                if (code >= 0 && code < (int)sf::Keyboard::KeyCount) {
                    input.bindAction(action, (sf::Keyboard::Key)code);
                }
            }
        }
    } catch (...) {}
}

static void applyDefaultBindings(InputManager& input) {
    if (input.keyFor("MoveUp") == sf::Keyboard::Key::Unknown) {
        input.bindAction("MoveUp", sf::Keyboard::Key::W);
        input.bindAction("MoveDown", sf::Keyboard::Key::S);
        input.bindAction("MoveLeft", sf::Keyboard::Key::A);
        input.bindAction("MoveRight", sf::Keyboard::Key::D);
        input.bindAction("Interact", sf::Keyboard::Key::E);
        input.bindAction("Shoot", sf::Keyboard::Key::Space);
        input.bindAction("Inventory", sf::Keyboard::Key::I);
        input.bindAction("Help", sf::Keyboard::Key::H);
        input.bindAction("RailTool", sf::Keyboard::Key::B);
        input.bindAction("Quit", sf::Keyboard::Key::Escape);
        input.bindAction("ToggleMoisture", sf::Keyboard::Key::M);
        input.bindAction("ToggleFertility", sf::Keyboard::Key::N);
        input.bindAction("ToggleRespawnUnits", sf::Keyboard::Key::T);
        input.bindAction("ToggleMinimap", sf::Keyboard::Key::U);
        input.bindAction("CycleMinimapScale", sf::Keyboard::Key::J);
        input.bindAction("ToggleMinimapViewRect", sf::Keyboard::Key::V);
        input.bindAction("ToggleMinimapEntities", sf::Keyboard::Key::G);
        input.bindAction("ToggleDeathPenalty", sf::Keyboard::Key::Y);
        input.bindAction("QuickSave", sf::Keyboard::Key::K);
        input.bindAction("QuickLoad", sf::Keyboard::Key::L);
        input.bindAction("Fertilize", sf::Keyboard::Key::F);
        input.bindAction("ToggleCodex", sf::Keyboard::Key::C);
        input.bindAction("CraftSalve", sf::Keyboard::Key::Q);
        input.bindAction("UseSalve", sf::Keyboard::Key::R);
        input.bindAction("ToggleRailOverlay", sf::Keyboard::Key::X);
        input.bindAction("CartRouteMode", sf::Keyboard::Key::Z); // new action
        input.bindAction("AssignLoader", sf::Keyboard::Key::Num1);
        input.bindAction("AssignUnloader", sf::Keyboard::Key::Num2);
        input.bindAction("Journal", sf::Keyboard::Key::J); // Phase 4 journal panel toggle (temporarily J; consider remap later)
    }
}

extern void LoadItemDefinitions(const std::string& path);
extern bool LoadCustomBindings(InputManager& input, const std::string& path);
extern void SaveCustomBindings(const InputManager& input, const std::string& path);

struct Tunables { nlohmann::json j; };
Tunables g_tunables; // remove static to allow accessor
nlohmann::json* g_getTunablesJson() { return g_tunables.j.is_null()? nullptr : &g_tunables.j; }
static void loadTunables(const std::string& path) {
    std::ifstream is(path); if(!is) { std::cerr << "No tunables file: "<<path<<"\n"; return; }
    try { is >> g_tunables.j; std::cerr << "Loaded tunables keys="<<g_tunables.j.size()<<"\n"; } catch(...) { std::cerr << "Failed tunables parse\n"; }
}

Game::Game()
: window(sf::VideoMode({1024u, 768u}), "Top-down Game Framework")
, camera(window.getDefaultView())
{
    resourceManager = std::make_unique<ResourceManager>();
    inputManager = std::make_unique<InputManager>();
    soundManager = std::make_unique<SoundManager>();
    loadBindings(*inputManager);
    applyDefaultBindings(*inputManager);
    LoadCustomBindings(*inputManager, "bindings.saved.json");
    LoadItemDefinitions("data/items_basic.json");
    loadTunables("data/tunables.json");
    // Apply global tunables that affect world systems (soil)
    if (auto *tj = g_getTunablesJson()) {
        if ((*tj).contains("soil")) {
            auto &sj = (*tj)["soil"];
            float moistTarget = sj.value("moisture_target", 0.3f);
            float moistDecay = sj.value("moisture_decay_per_sec", 0.02f);
            float fertTarget = sj.value("fertility_target", 0.5f);
            float fertRegen = sj.value("fertility_regen_per_sec", 0.005f);
            // defer applying until PlayState constructed and map accessible (handled inside PlayState after creation if needed)
        }
    }
    currentState = std::make_unique<PlayState>(*this);
    window.setFramerateLimit(60);
}

Game::~Game() = default;

// detection: does sf::Event have a `.type` member (SFML2)?
template<typename T, typename = void>
struct has_event_type_member : std::false_type {};
template<typename T>
struct has_event_type_member<T, std::void_t<decltype(std::declval<const T&>().type)>> : std::true_type {};

// detection: is `event == sf::Event::Closed{}` well-formed?
template<typename T, typename U, typename = void>
struct has_equality_with : std::false_type {};
template<typename T, typename U>
struct has_equality_with<T, U, std::void_t<decltype(std::declval<const T&>() == std::declval<const U&>())>> : std::true_type {};

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
        // Simplified: rely on action mapping (Quit) to close window; ignore platform close to avoid SFML version differences.
        if (currentState) currentState->handleEvent(event);
        optEvent = window.pollEvent();
    }
}

void Game::update(sf::Time dt) {
    // sample current keyboard state so entities can query input during update
    inputManager->poll();

    if (currentState) currentState->update(dt);
}

void Game::render() {
    window.clear(sf::Color(50, 50, 70));
    if (currentState) currentState->draw();
    window.display();
}

ResourceManager& Game::resources() { return *resourceManager; }
InputManager& Game::input() { return *inputManager; }
SoundManager& Game::sound() { return *soundManager; }
void Game::setState(std::unique_ptr<State> s) { currentState = std::move(s); }
void Game::pushTemporaryState(std::unique_ptr<State> s) { savedState = std::move(currentState); currentState = std::move(s); }
void Game::popTemporaryState() { if (savedState) { currentState = std::move(savedState); } }

void Game::step(float dtSeconds) {
    // Poll input snapshot (no events in headless mode yet)
    inputManager->poll();
    if (currentState) currentState->update(sf::seconds(dtSeconds));
}