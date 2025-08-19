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
#include <unordered_map>

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
    struct SpawnZone { sf::Vector2f center; float radius; float interval; float timer; int maxAlive; };

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

    // respawn marker visuals
    bool showRespawnMarker = true; // toggle with P
    float respawnMarkerTime = 0.f;
    bool showRespawnDistance = true; // toggle with O
    bool respawnDistanceInTiles = true; // toggle units with T (true=tiles, false=pixels)
    bool showMinimap = true; // toggle with U
    bool enableDeathPenalty = true; // toggle with Y
    float minimapTilePixel = 2.f; // minimap tile pixel size, cycle with J (2,3,4)
    bool showMinimapViewRect = true; // toggle with V
    bool showMinimapEntities = true; // toggle with G (draw entity icons)
    bool showHelpOverlay = false; // toggle with H
    // TODO: future minimap features: zoom levels, fog gradient, entity icons

    bool tryMovePlayer(const sf::Vector2f& desired);
    void syncRailsWithMap();
    void spawnProjectile(std::unique_ptr<Entity> p);

    DialogManager dialog;

    // Harvested crops counter and fertilizer unlock flag
    int harvestedCropsCount = 0;
    bool fertilizerUnlocked = true; // set false initially for progression
    bool codexEnabled = false; // future: display crop info dialog
    std::unordered_map<std::string, std::vector<std::string>> cropCodex; // id -> lines
    bool showCodex = false; // toggle view

    std::vector<SpawnZone> spawnZones; // hostile spawn zones

    // Hostile spawn system
    std::vector<sf::Vector2f> hostileSpawnPoints; // predefined spawn zones
    float hostileSpawnTimer = 0.f;
    float hostileSpawnInterval = 8.f; // seconds between spawn attempts
    int   maxHostiles = 6; // cap active hostiles
    float minSpawnDistance = 300.f; // player distance requirement

    // Threat scaling placeholder
    float threatLevel = 0.f; // increases over time & movement
    float threatTime = 0.f;  // elapsed time accumulator
    sf::Vector2f lastPlayerPos; // for distance-based contribution
    float hostileSpawnIntervalBase = 8.f; // base before scaling

    // Farming onboarding demo
    bool farmingDemoActive = true; // enabled first run
    int farmingDemoStage = 0; // 0 intro,1 water,2 fertilize (or skip),3 harvest,4 done
    bool farmingDemoCompleted = false;
    std::vector<sf::Vector2u> farmingDemoTiles; // tiles selected for demo plot
    bool demoWatered = false;
    bool demoFertilized = false;
    bool demoHarvested = false;
};