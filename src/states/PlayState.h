#pragma once
#include "../core/State.h"
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>

// include concrete entity headers so PlayState's members are complete types
#include "../entities/Entity.h"
#include "../entities/Player.h"
#include "../world/TileMap.h"
#include "../systems/Dialog.h"
#include "../ui/InventoryUI.h"
#include "../tools/RailTool.h"

class Game;

class PlayState : public State {
public:
    PlayState(Game& game);
    void handleEvent(const sf::Event&) override;
    void update(sf::Time) override;
    void draw() override;

    // save/load hooks
    void saveGame(const std::string& path);
    void loadGame(const std::string& path);
private:
    Game& game;
    std::unique_ptr<Player> player;
    std::vector<std::unique_ptr<Entity>> entities;
    sf::View view;
    TileMap map;
    void handleInteractions();
    // helpers
    bool tryMovePlayer(const sf::Vector2f& desired);

    // dialog system
    DialogManager dialog;

    // UI
    std::unique_ptr<InventoryUI> inventoryUI;

    // tools
    std::unique_ptr<RailTool> railTool;

    // synchronize Rail entities with TileMap state
    void syncRailsWithMap();

    // world projectiles
    std::vector<std::unique_ptr<Entity>> worldProjectiles;

    // spawn a projectile in the world (added by player or NPCs)
    void spawnProjectile(std::unique_ptr<Entity> p);

    // death / respawn handling
    bool playerDead = false;
    float respawnTimer = 0.f;
    float respawnDelay = 3.f; // seconds until respawn
    sf::Vector2f respawnPos;
};