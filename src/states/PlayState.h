#pragma once
#include "../core/State.h"
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include "../world/TileMap.h"
#include "../systems/Dialog.h"
#include "../entities/Player.h"
#include "../ui/InventoryUI.h" // added for complete type
#include "../tools/RailTool.h"  // added for complete type

class Game;
class Projectile;
class Altar;

class PlayState : public State {
public:
    PlayState(Game& game);
    void handleEvent(const sf::Event&) override;
    void update(sf::Time) override;
    void draw() override;
private:
    Game& game;
    std::unique_ptr<Player> player;
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::unique_ptr<Entity>> worldProjectiles;
    sf::View view;
    TileMap map;

    // UI & tools
    std::unique_ptr<InventoryUI> inventoryUI;
    std::unique_ptr<RailTool> railTool;

    // Respawn mechanics
    sf::Vector2f respawnPos;
    bool playerDead = false;
    float respawnTimer = 0.f;
    float respawnDelay = 3.f;

    // Hidden realm
    bool hiddenRealmActive = false;

    bool tryMovePlayer(const sf::Vector2f& desired);
    void syncRailsWithMap();
    void spawnProjectile(std::unique_ptr<Entity> p);

    DialogManager dialog;
};