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
#include "../systems/Quest.h" // added for quest types

class Game;
class Projectile;
class Altar;
class HostileNPC; // forward declaration for spawnHostile
class Cart; // forward declaration for rail carts

class PlayState : public State {
public:
    PlayState(Game& game);
    ~PlayState(); // ensure complete Cart type in cpp
    void handleEvent(const sf::Event&) override;
    void update(sf::Time) override;
    void draw() override;

    void saveGame(const std::string& path);
    void loadGame(const std::string& path);
private:
    struct CombatText { sf::Text text; sf::Vector2f vel; float lifetime; };
    struct SpawnZone { sf::Vector2f center; float radius; float interval; float timer; int maxAlive; };
    struct Directive { std::string id; std::string text; bool satisfied=false; bool hidden=false; int progress=0; int target=0; float completedAt=-1.f; }; // added completedAt for HUD fade timing
    struct HarvestFX { sf::Vector2f pos; float elapsed=0.f; float duration=0.45f; int phase=0; int yield=0; bool active=true; };

    Game& game;
    std::unique_ptr<Player> player;
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<std::unique_ptr<Entity>> worldProjectiles;
    std::vector<std::unique_ptr<Cart>> carts; // rail carts managed separately
    sf::View view;
    TileMap map;
    bool moistureOverlay = false; // toggle with M
    bool fertilityOverlay = false; // toggle with N
    std::vector<CombatText> combatTexts; // floating damage numbers

    // UI & tools
    std::unique_ptr<InventoryUI> inventoryUI;
    std::unique_ptr<RailTool> railTool;

    // Cart route editing (prototype)
    bool cartRouteMode = false; // toggle with CartRouteMode action (Z)
    Cart* activeCart = nullptr; // cart being edited
    bool loaderMode = false; // assign loader tile next click
    bool unloaderMode = false; // assign unloader tile next click
    sf::Vector2u loaderTile{UINT32_MAX,UINT32_MAX};
    sf::Vector2u unloaderTile{UINT32_MAX,UINT32_MAX};
    float logisticsTimer = 0.f; // tick accumulator

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
    bool showRailOverlay = true; // toggle with X
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
    float hostileSpawnInterval = 5.f;
    int   maxHostiles = 5; // cap active hostiles
    float minSpawnDistance = 300.f; // player distance requirement

    // Threat scaling placeholder
    float threatLevel = 0.f; // increases over time / kills
    float threatTime = 0.f;  // elapsed time accumulator
    sf::Vector2f lastPlayerPos; // for distance-based contribution
    float hostileSpawnIntervalBase = 8.f; // base before scaling
    float threatToIntervalFactor = 0.05f; // tunable scaling to reduce interval
    float threatToMaxHostilesFactor = 0.02f; // tunable scaling to raise cap
    float tankSpawnChance = 0.0f; // increases with threat
    bool hostileSpawningEnabled = false; // NEW: disable random hostile spawning (debug request)

    // Farming onboarding demo
    bool farmingDemoActive = true; // enabled first run
    int farmingDemoStage = 0; // 0 intro,1 water,2 fertilize (or skip),3 harvest,4 done
    bool farmingDemoCompleted = false;
    std::vector<sf::Vector2u> farmingDemoTiles; // tiles selected for demo plot
    bool demoWatered = false;
    bool demoFertilized = false;
    bool demoHarvested = false;

    // quest system minimal
    std::vector<Quest> activeQuests;
    int cartItemsMoved = 0; // progress counter for logistics quest
    std::vector<Directive> directives; // active directives (Phase 4 scaffold)
    bool showJournal = false; // toggle future journal UI
    size_t lastEntityCount = 0; // track entity count to detect new crop plantings
    int questChainStage = 0; // Phase 4 quest chain progression (0=plant,1=harvest,2=rail,3=done)
    float hudTime = 0.f; // NEW: clock for directive fade timing

    // Hold-to-Harvest state
    bool harvestingActive = false; // player holding key
    float harvestHoldTime = 0.f; // accumulated hold time
    float harvestStageInterval = 0.12f; // 120ms between queued actions
    float harvestStageTimer = 0.f; // timer until next harvest action
    sf::Vector2u lastHarvestTile{UINT32_MAX, UINT32_MAX};
    int harvestRadius = 1; // 3x3 (radius 1) sweep

    // Magnet pickup settings
    float magnetRadius = 2.5f; // tiles
    float magnetAcceleration = 900.f; // px/s^2 toward player
    float magnetMaxSpeed = 600.f; // cap

    void updateQuests();
    void incrementQuestProgress(const std::string& objectiveId, int amount=1);
    void evaluateDirectives(); // Phase 4
    void updateQuestChain(); // Phase 4 chain logic
    HostileNPC* spawnHostile(const sf::Vector2f& pos);

    void attemptPlanting(const sf::Vector2f& worldPos); // RESTORED: planting logic
    void clampViewCenter(); // ensure camera stays in map bounds
    void processHoldToHarvest(sf::Time dt);

    std::vector<HarvestFX> harvestFxList; // active harvest effects
};