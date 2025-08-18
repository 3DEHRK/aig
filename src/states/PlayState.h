#pragma once
#include "../core/State.h"
#include <vector>
#include <memory>
#include <string>
#include <SFML/Graphics.hpp>
#include "../world/TileMap.h"
#include "../systems/Dialog.h"
#include "../entities/Player.h"
#include "../ui/InventoryUI.h"
#include "../tools/RailTool.h"

class Game;
class Projectile;
class Altar;

class PlayState : public State {
public:
    PlayState(Game& game);
    void handleEvent(const sf::Event&) override;
    void update(sf::Time) override;
    void draw() override;

    void saveGame(const std::string& path);
    void loadGame(const std::string& path);
private:
    struct CombatText { sf::Text text; sf::Vector2f vel; float lifetime; };

    Game& game;
    std::unique_ptr<Player> player;
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::unique_ptr<Entity>> worldProjectiles;
    sf::View view;
    TileMap map;
    bool moistureOverlay = false; // toggle with M
    bool fertilityOverlay = false; // toggle with N
    std::vector<CombatText> combatTexts; // floating damage numbers

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

    // projectile firing control
    float projectileCooldown = 0.4f;
    float timeSinceLastProjectile = 0.f;

    // respawn marker visuals & settings
    bool showRespawnMarker = true; // toggle with P
    float respawnMarkerTime = 0.f;
    bool showRespawnDistance = true; // toggle with O
    bool respawnDistanceInTiles = true; // toggle units with T (true=tiles, false=pixels)

    // minimap & UI toggles
    bool showMinimap = true; // toggle with U
    bool enableDeathPenalty = true; // toggle with Y
    float minimapTilePixel = 2.f; // minimap tile pixel size, cycle with J (2,3,4)
    bool showMinimapViewRect = true; // toggle with V
    bool showMinimapEntities = true; // toggle with G (draw entity icons)
    bool showHelpOverlay = false; // toggle with H
    // TODO: future minimap features: zoom levels, fog gradient, legend, better icons

    bool tryMovePlayer(const sf::Vector2f& desired);
    void syncRailsWithMap();
    void spawnProjectile(std::unique_ptr<Entity> p);

    DialogManager dialog;
};