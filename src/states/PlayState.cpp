#include "PlayState.h"
#include "../core/Game.h"
#include "../resources/ResourceManager.h"
#include "../entities/Player.h"
#include "../entities/NPC.h"
#include "../entities/ItemEntity.h"
#include "../entities/Crop.h"
#include "../entities/Rail.h"
#include "../entities/HostileNPC.h"
#include "../entities/HiddenLocation.h"
#include "../entities/Altar.h"
#include "../input/InputManager.h"
#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include <algorithm>
#include <cmath>
#include "../systems/SaveGame.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include "../items/Item.h"
#include "../tools/RailTool.h"
#include "../world/TileMap.h"
#include "../entities/Projectile.h"
#include <sstream>
#include "../entities/Entity.h" // for resolveAxis helper
#include <unordered_map>
#include <random> // added for mt19937 and uniform_real_distribution
#include "../entities/Cart.h" // cart integration
#include "../systems/Quest.h"

// rect center for current SFML (uses position/size members)
static sf::Vector2f rect_center(const sf::FloatRect& r) {
    return { r.position.x + r.size.x * 0.5f, r.position.y + r.size.y * 0.5f };
}

// deterministic RNG for hostile variant spawning
static std::mt19937 g_hostileRng(1337u);
static std::uniform_real_distribution<float> g_hostileDist(0.f,1.f);
static float rand01() { return g_hostileDist(g_hostileRng); }

PlayState::PlayState(Game& g)
: game(g), view(g.getWindow().getDefaultView()), map(50, 30, 32)
{
    // Load crop configs before creating crops
    Crop::loadConfigs(game.resources(), "data/crops.json");
    map.generateTestMap();
    std::cerr << "[PlayState] Setting rail texture path=assets/textures/entities/tiles/rail.png\n";
    map.setRailTexture(game.resources(), "assets/textures/entities/tiles/rail.png");
    if (auto *tj = g_getTunablesJson()) {
        if ((*tj).contains("soil")) {
            auto &sj = (*tj)["soil"];
            map.setSoilTunables(sj.value("moisture_target",0.3f), sj.value("moisture_decay_per_sec",0.02f), sj.value("fertility_target",0.5f), sj.value("fertility_regen_per_sec",0.005f));
        }
    }

    // try to set a font for dialog (user should place Arial at assets/fonts/arial.ttf)
    try {
        auto &f = game.resources().font("assets/fonts/arial.ttf");
        dialog.setFont(std::shared_ptr<sf::Font>(&f, [](sf::Font*){}));
    } catch (...) {}

    player = std::make_unique<Player>(game.input(), game.resources());
    std::cerr << "[PlayState] Creating player with texture assets/textures/entities/player_idle.png\n";
    respawnPos = player->position();
    // give the player some sample seeds for testing — start with more seeds for reliable testing
    for (int i = 0; i < 5; ++i) {
        player->inventory().addItem(std::make_shared<Item>("seed_wheat", "Wheat Seed", "Seed", 1));
    }

    // now that player exists, create inventoryUI with player's inventory reference
    inventoryUI = std::make_unique<InventoryUI>(game.resources(), player->inventory());

    entities.push_back(std::make_unique<NPC>(sf::Vector2f(700.f, 380.f)));
    auto sample = std::make_shared<Item>("apple_01", "Apple", "A juicy apple", 1);
    entities.push_back(std::make_unique<ItemEntity>(sample, sf::Vector2f(600.f, 380.f)));

    // spawn crops
    entities.push_back(std::make_unique<Crop>(game.resources(), map, sf::Vector2f(300.f, 300.f), "wheat", 3, 5.f));
    entities.push_back(std::make_unique<Crop>(game.resources(), map, sf::Vector2f(340.f, 300.f), "wheat", 3, 7.f));

    // add sample rail pieces in a small area
    entities.push_back(std::make_unique<Rail>(game.resources(), sf::Vector2f(200.f, 200.f), map.tileSize()));
    entities.push_back(std::make_unique<Rail>(game.resources(), sf::Vector2f(232.f, 200.f), map.tileSize()));
    entities.push_back(std::make_unique<Rail>(game.resources(), sf::Vector2f(264.f, 200.f), map.tileSize()));

    // create a test altar
    entities.push_back(std::make_unique<Altar>(game.resources(), sf::Vector2f(900.f, 600.f)));
    if (auto altar = dynamic_cast<Altar*>(entities.back().get())) {
        altar->setRequiredItems({"dongle_mysterious"});
    }

    // spawn a hostile NPC targeting the player
    entities.push_back(std::make_unique<HostileNPC>(sf::Vector2f(400.f, 300.f), player.get()));
    if (auto h = dynamic_cast<HostileNPC*>(entities.back().get())) h->setTileMap(&map);

    // add a hidden location test marker at tile (10,10)
    unsigned hx = 10, hy = 10;
    float tsf = (float)map.tileSize();
    sf::Vector2f hpos((float)hx * tsf + tsf * 0.5f, (float)hy * tsf + tsf * 0.5f);
    entities.push_back(std::make_unique<HiddenLocation>(map, hx, hy));

    // ensure NPCs get a pointer to the world TileMap for simple collision checks
    for (auto &e : entities) {
        if (auto npc = dynamic_cast<NPC*>(e.get())) npc->setTileMap(&map);
    }

    // initialize rail tool
    railTool = std::make_unique<RailTool>(game.resources(), map);

    // load crop codex
    try {
        std::ifstream cs("data/crop_codex.json");
        if (cs) { nlohmann::json cj; cs >> cj; if (cj.contains("crops")) {
            for (auto &entry : cj["crops"]) {
                if (entry.contains("id") && entry.contains("lines") && entry["lines"].is_array()) {
                    std::vector<std::string> lines;
                    for (auto &ln : entry["lines"]) lines.push_back(ln.get<std::string>());
                    cropCodex[entry["id"].get<std::string>()] = lines;
                }
            }
            codexEnabled = true;
        }} // FIX: added missing brace to close outer 'if (cs)'
    } catch(...) { }

    // define simple hostile spawn zones (corners + center radius ring)
    float W = map.worldSize().x; float H = map.worldSize().y;
    hostileSpawnPoints = {
        {64.f,64.f}, {W-64.f,64.f}, {64.f,H-64.f}, {W-64.f,H-64.f},
        {W*0.5f,64.f}, {W*0.5f,H-64.f}, {64.f,H*0.5f}, {W-64.f,H*0.5f}
    };

    lastPlayerPos = player->position();

    // Initialize farming onboarding demo plot (3 tiles near player spawn)
    if (farmingDemoActive) {
        unsigned tsz = map.tileSize();
        sf::Vector2f p0 = player->position();
        unsigned px = (unsigned)std::floor(p0.x / tsz);
        unsigned py = (unsigned)std::floor(p0.y / tsz);
        for (int ox=0; ox<3; ++ox) {
            unsigned tx = px + ox + 1; unsigned ty = py + 1;
            if (tx < map.width() && ty < map.height()) {
                map.setTile(tx,ty, TileMap::Plantable);
                farmingDemoTiles.push_back({tx,ty});
            }
        }
        dialog.start({ "Welcome! Let's plant a crop.", "Press E near soil with a seed to plant." });
        farmingDemoStage = 0;
    }

    // AFTER farming demo init add a prototype cart on a short horizontal rail segment
    {
        auto cart = std::make_unique<Cart>(game.resources(), sf::Vector2f(200.f + map.tileSize()*0.5f, 200.f + map.tileSize()*0.5f), map.tileSize());
        cart->setTileMap(&map);
        // waypoints across the three sample rails placed earlier (x=200,232,264)
        cart->addWaypoint({ (unsigned)(200 / map.tileSize()), (unsigned)(200 / map.tileSize()) });
        cart->addWaypoint({ (unsigned)(232 / map.tileSize()), (unsigned)(200 / map.tileSize()) });
        cart->addWaypoint({ (unsigned)(264 / map.tileSize()), (unsigned)(200 / map.tileSize()) });
        cart->setLoop(true);
        carts.push_back(std::move(cart));
    }

    // initialize starter logistics quest
    {
        Quest q; q.id = "starter_logistics"; q.title = "Automate a Wheat Haul"; q.description = "Move 5 wheat seeds via cart (loader -> unloader).";
        QuestObjective o; o.id = "move_item_via_cart"; o.target = 5; q.objectives.push_back(o);
        activeQuests.push_back(q);
    }
    // Phase 4: seed initial directives
    directives.push_back({"plant_seed", "Plant a seed", false, false, 0, 1});
    directives.push_back({"harvest_crops", "Harvest 5 crops", false, true, 0, 5}); // hidden until plant_seed done
    directives.push_back({"build_rail", "Place a rail segment", false, true, 0, 1}); // hidden until harvest_crops done
    // add starter chain quest (three steps mirroring directives) if not already present
    {
        bool have = false; for (auto &q : activeQuests) if (q.id == "starter_chain") { have = true; break; }
        if (!have) {
            Quest q; q.id = "starter_chain"; q.title = "Basics of Settlement"; q.description = "Complete the fundamental setup steps.";
            QuestObjective a; a.id="plant_seed"; a.target = 1; q.objectives.push_back(a);
            QuestObjective b; b.id="harvest_crops"; b.target = 5; q.objectives.push_back(b);
            QuestObjective c; c.id="build_rail"; c.target = 1; q.objectives.push_back(c);
            activeQuests.push_back(q);
        }
    }
}

PlayState::~PlayState() = default;

void PlayState::handleEvent(const sf::Event& /*ev*/) {
    // Per-frame polling is used in update; events can be forwarded here when needed.
}

bool PlayState::tryMovePlayer(const sf::Vector2f& desired) {
    // Improved AABB vs tile collision using axis resolution and resolveAxis helper
    if (!player) return false;
    sf::FloatRect pb = player->getBounds();
    auto sweepAxis = [&](float& move, bool horizontal){
        if (move == 0.f) return;
        sf::FloatRect b = player->getBounds();
        // Expand query region to cover movement path
        float minX = b.position.x + (horizontal? std::min(0.f, move) : 0.f);
        float maxX = b.position.x + b.size.x + (horizontal? std::max(0.f, move) : 0.f);
        float minY = b.position.y + (!horizontal? std::min(0.f, move) : 0.f);
        float maxY = b.position.y + b.size.y + (!horizontal? std::max(0.f, move) : 0.f);
        unsigned ts = map.tileSize();
        int tx0 = std::max(0, (int)std::floor(minX / ts));
        int tx1 = std::min((int)map.width()-1, (int)std::floor((maxX-0.0001f) / ts));
        int ty0 = std::max(0, (int)std::floor(minY / ts));
        int ty1 = std::min((int)map.height()-1, (int)std::floor((maxY-0.0001f) / ts));
        float posMin = horizontal? b.position.x : b.position.y;
        float posMax = horizontal? (b.position.x + b.size.x) : (b.position.y + b.size.y);
        for (int ty = ty0; ty <= ty1; ++ty) {
            for (int tx = tx0; tx <= tx1; ++tx) {
                if (!map.isTileSolid(tx,ty)) continue;
                sf::FloatRect tileRect({(float)tx*ts,(float)ty*ts},{(float)ts,(float)ts});
                float blockMin = horizontal? tileRect.position.x : tileRect.position.y;
                float blockMax = horizontal? (tileRect.position.x + tileRect.size.x) : (tileRect.position.y + tileRect.size.y);
                float adjusted = resolveAxis(posMin, posMax, move, blockMin, blockMax);
                if (std::abs(adjusted) < std::abs(move)) move = adjusted; // clamped
                if (move == 0.f) return; // fully blocked
            }
        }
    };
    float mx = desired.x; float my = desired.y;
    sweepAxis(mx, true);
    player->applyMove({mx,0.f});
    sweepAxis(my, false);
    player->applyMove({0.f,my});
    return true;
}

void PlayState::update(sf::Time dt) {
    // Global quit shortcut
    if (game.input().actionPressed("Quit")) { game.getWindow().close(); return; }

    hudTime += dt.asSeconds(); // accumulate for directive fade timing

    // journal toggle (Phase 4)
    if (game.input().actionPressed("Journal")) { showJournal = !showJournal; }
    // toggle cart route mode (Z) and set activeCart
    if (game.input().actionPressed("CartRouteMode")) {
        cartRouteMode = !cartRouteMode;
        if (cartRouteMode) {
            if (!carts.empty()) { activeCart = carts[0].get(); }
        } else {
            activeCart = nullptr;
            loaderMode = false; unloaderMode = false;
        }
    }
    if (game.input().actionPressed("AssignLoader")) { loaderMode = true; unloaderMode = false; }
    if (game.input().actionPressed("AssignUnloader")) { unloaderMode = true; loaderMode = false; }

    // if a dialog is active, update dialog and skip movement/interactions
    if (dialog.active()) {
        dialog.update(game.input(), dt);
        // still allow dialog to be drawn; update early-return so game world pauses
        // if this dialog is a hidden-realm confirmation, allow the state to continue
        if (!hiddenRealmActive) {
            // per-frame input housekeeping
            game.input().clearFrame();
            return;
        }
    }

    // toggle inventory UI (use action mapping)
    if (game.input().actionPressed("Inventory")) {
        if (inventoryUI) inventoryUI->toggle();
    }

    // toggle moisture overlay
    if (game.input().actionPressed("ToggleMoisture")) { moistureOverlay = !moistureOverlay; }
    if (game.input().actionPressed("ToggleFertility")) { fertilityOverlay = !fertilityOverlay; }
    // distance unit toggle (tiles/pixels)
    if (game.input().actionPressed("ToggleRespawnUnits")) { respawnDistanceInTiles = !respawnDistanceInTiles; }
    if (game.input().actionPressed("ToggleMinimap")) { showMinimap = !showMinimap; }
    // add minimap scale cycle (J)
    if (game.input().actionPressed("CycleMinimapScale")) {
        // cycle through 2,3,4 then back
        if (minimapTilePixel < 4.f) minimapTilePixel += 1.f; else minimapTilePixel = 2.f;
    }
    // toggle minimap view rectangle
    if (game.input().actionPressed("ToggleMinimapViewRect")) { showMinimapViewRect = !showMinimapViewRect; }
    // toggle entity icons on minimap
    if (game.input().actionPressed("ToggleMinimapEntities")) { showMinimapEntities = !showMinimapEntities; }
    if (game.input().actionPressed("Help")) { showHelpOverlay = !showHelpOverlay; }
    // debug key: toggle death penalty mode?
    if (game.input().actionPressed("ToggleDeathPenalty")) { enableDeathPenalty = !enableDeathPenalty; }
    // quick save/load
    if (game.input().actionPressed("QuickSave")) { saveGame("SaveGame.json"); SaveCustomBindings(game.input(), "bindings.saved.json"); }
    if (game.input().actionPressed("QuickLoad")) { loadGame("SaveGame.json"); }

    // ensure UI gets per-frame input handling
    if (inventoryUI) inventoryUI->update(game.input(), game.getWindow(), dt);

    // toggle rail build tool
    if (game.input().actionPressed("RailTool")) {
        if (railTool) {
            railTool->toggle();
            std::cerr << "Rail tool toggled. enabled=" << (railTool->enabled ? 1 : 0) << "\n";
        }
    }

    // process input into player (sets vel & flags)
    player->update(dt);
    // Hold-to-Harvest detection (reuse Interact action Down)
    bool interactDown = game.input().actionDown("Interact");
    if (interactDown) {
        if (!harvestingActive) { harvestingActive = true; harvestHoldTime = 0.f; harvestStageTimer = 0.f; }
    } else if (harvestingActive) {
        harvestingActive = false; lastHarvestTile = {UINT32_MAX, UINT32_MAX};
    }
    if (harvestingActive) processHoldToHarvest(dt);

    // If player is riding a cart, override their movement to zero (cart carries them)
    bool playerRiding = false;
    for (auto &c : carts) { if (c->hasRider() && c->getRider() == player.get()) { playerRiding = true; break; } }
    // Planting attempt on Interact if no entity targeted will be processed after click logic using attemptPlanting
    timeSinceLastProjectile += dt.asSeconds();
    // fire projectile using action mapping (ensure block closed properly)
    if (game.input().actionPressed("Shoot") && timeSinceLastProjectile >= projectileCooldown) {
        sf::Vector2f dir = player->computeDesiredMove(sf::seconds(1.f));
        if (dir.x == 0 && dir.y == 0) dir = {1.f,0.f};
        float len = std::sqrt(dir.x*dir.x + dir.y*dir.y);
        if (len > 0.f) dir /= len;
        float dmg = player->baseDamage();
        float projSpeed = 300.f; float projKnock = 0.f; float projLife = 2.f;
        if (auto *tj = g_getTunablesJson()) {
            if ((*tj).contains("projectile")) {
                auto &pj = (*tj)["projectile"];
                if (pj.contains("speed")) projSpeed = pj["speed"].get<float>();
                if (pj.contains("knockback")) projKnock = pj["knockback"].get<float>();
                if (pj.contains("lifetime")) projLife = pj["lifetime"].get<float>();
            }
        }
        spawnProjectile(std::make_unique<Projectile>(player->position(), dir * projSpeed, projSpeed, projLife, dmg, projKnock));
        timeSinceLastProjectile = 0.f;
    }

    // death handling & penalty (moved below projectile fire)
    if (playerDead) {
        respawnTimer += dt.asSeconds();
        if (respawnTimer >= respawnDelay) {
            if (enableDeathPenalty && player) {
                for (auto &it : player->inventory().items()) {
                    if (!it) continue;
                    if (it->id.rfind("seed_",0)==0) continue;
                    if (it->stackSize <= 0) continue;
                    int lose = std::max(1, it->stackSize / 10);
                    it->stackSize = std::max(0, it->stackSize - lose);
                }
            }
            playerDead = false;
            player->healToFull();
            player->setPosition(respawnPos);
            player->triggerInvulnerability(2.0f);
            player->resetLifeStats();
        }
    }

    // update soil simulation
    map.updateSoil(dt);

    // Magnet Pickup: attract loose items within radius
    {
        float ts = (float)map.tileSize();
        float radiusPx = magnetRadius * ts;
        float radiusSq = radiusPx * radiusPx;
        sf::Vector2f pp = player->position();
        for (auto &e : entities) {
            if (auto itemEnt = dynamic_cast<ItemEntity*>(e.get())) {
                if (itemEnt->collected()) continue;
                sf::FloatRect b = itemEnt->getBounds();
                sf::Vector2f center{b.position.x + b.size.x*0.5f, b.position.y + b.size.y*0.5f};
                sf::Vector2f d = pp - center;
                float distSq = d.x*d.x + d.y*d.y;
                if (distSq <= radiusSq) {
                    float dist = std::sqrt(distSq);
                    if (dist > 1.f) {
                        sf::Vector2f dir = d / dist;
                        // approximate velocity stored in ItemEntity; need access -> dynamic_cast and modify shape directly
                        // We added velocity field; adjust via friend-like approach using public startMagnet & move by adding to velocity.
                        // Reinterpret by adding small impulse via direct move each frame.
                        float accel = magnetAcceleration;
                        float step = accel * dt.asSeconds();
                        // clamp speed by maxSpeed * dt for displacement-based integration
                        sf::Vector2f move = dir * step * 0.016f; // small fraction; actual move happens in item update currently minimal
                        // Instead apply immediate position shift toward player at controlled speed
                        float maxStep = magnetMaxSpeed * dt.asSeconds();
                        float desired = std::min(maxStep, dist);
                        sf::Vector2f shift = dir * desired * 0.5f; // half-speed to allow smoothness
                        // Directly move via shape (need accessor) -> we lack setter; hack: interact() uses bounds only. Replace by const_cast? Better: extend ItemEntity with move method.
                        // Temporary: if close enough, collect.
                        if (dist < 28.f) {
                            itemEnt->interact(player.get());
                        } else {
                            // fallback: call startMagnet so its internal update drifts (limited without player pos). For now teleport small fraction.
                            itemEnt->startMagnet();
                        }
                    } else {
                        itemEnt->interact(player.get());
                    }
                }
            }
        }
    }

    player->updateHealthRegen(dt);

    // compute desired movement and attempt collision-aware moves
    sf::Vector2f desired = player->computeDesiredMove(dt);
    if (playerRiding) desired = {0.f,0.f}; // movement suppressed while riding
    tryMovePlayer(desired);
    // update camera center to follow player
    view.setCenter(player->position());
    clampViewCenter();

    // mark explored area around player
    {
        float ts = (float)map.tileSize();
        sf::Vector2f p = player->position();
        int px = (int)std::floor(p.x / ts);
        int py = (int)std::floor(p.y / ts);
        int radius = 6; // tiles radius to reveal
        for (int dy=-radius; dy<=radius; ++dy){
            for (int dx=-radius; dx<=radius; ++dx){
                int tx = px + dx; int ty = py + dy;
                if (dx*dx + dy*dy <= radius*radius) map.markExplored((unsigned)tx,(unsigned)ty);
            }
        }
    }

    // THREAT & HOSTILE SPAWN SYSTEM ------------------------------------------------
    if (hostileSpawningEnabled) { // gated by toggle (default off per debug request)
        float ds = dt.asSeconds();
        sf::Vector2f cur = player->position();
        float moveDist = std::hypot(cur.x - lastPlayerPos.x, cur.y - lastPlayerPos.y);
        lastPlayerPos = cur;
        threatLevel += ds * 0.25f + moveDist * 0.002f; // tuned scaling
        if (threatLevel > 50.f) threatLevel = 50.f;
        hostileSpawnInterval = hostileSpawnIntervalBase * std::max(0.25f, 1.f - threatLevel * threatToIntervalFactor);
        maxHostiles = 5 + (int)std::floor(threatLevel * threatToMaxHostilesFactor * 5.f);
        if (maxHostiles > 14) maxHostiles = 14;
        tankSpawnChance = std::min(0.5f, threatLevel * 0.01f);
        hostileSpawnTimer += ds;
        int active = 0; for (auto &e : entities) if (dynamic_cast<HostileNPC*>(e.get())) ++active;
        if (hostileSpawnTimer >= hostileSpawnInterval && active < maxHostiles) {
            hostileSpawnTimer = 0.f;
            std::vector<sf::Vector2f> candidates;
            for (auto &pt : hostileSpawnPoints) {
                sf::Vector2f d = pt - cur;
                if (d.x*d.x + d.y*d.y >= minSpawnDistance*minSpawnDistance) candidates.push_back(pt);
            }
            if (!candidates.empty()) {
                float r = rand01();
                sf::Vector2f sp = candidates[(size_t)(r * candidates.size()) % candidates.size()];
                spawnHostile(sp);
            }
        }
    }

    for (auto& e : entities) e->update(dt);
    // update carts
    for (auto &c : carts) c->update(dt);
    // update projectiles
    for (auto &p : worldProjectiles) p->update(dt);
    // projectile lifetime & collision vs hostile NPCs (fixed braces & logic)
    for (auto it = worldProjectiles.begin(); it != worldProjectiles.end(); ) {
        bool remove = false;
        if (auto proj = dynamic_cast<Projectile*>(it->get())) {
            if (proj->expired()) {
                remove = true;
            } else {
                sf::FloatRect pb = proj->getBounds();
                for (auto &e : entities) {
                    if (auto hostile = dynamic_cast<HostileNPC*>(e.get())) {
                        sf::FloatRect hb = hostile->getBounds();
                        bool overlap = !(pb.position.x + pb.size.x < hb.position.x ||
                                         hb.position.x + hb.size.x < pb.position.x ||
                                         pb.position.y + pb.size.y < hb.position.y ||
                                         hb.position.y + hb.size.y < pb.position.y);
                        if (overlap) {
                            // apply damage
                            hostile->takeDamage(proj->damage);
                            // combat text
                            try {
                                auto &f = game.resources().font("assets/fonts/arial.ttf");
                                sf::Text dmgTxt(f, std::to_string((int)proj->damage), 14u);
                                dmgTxt.setFillColor(sf::Color::White);
                                dmgTxt.setPosition({hb.position.x + hb.size.x*0.5f, hb.position.y - 10.f});
                                CombatText ct{dmgTxt, {0.f,-30.f}, 0.9f};
                                combatTexts.push_back(ct);
                            } catch(...) {}
                            proj->kill();
                            remove = true;
                            break;
                        }
                    }
                }
            }
        }
        if (remove) it = worldProjectiles.erase(it); else ++it;
    }

    // cull dead hostile NPCs (and spawn drops)
    for (auto it = entities.begin(); it != entities.end(); ) {
        bool erase = false;
        if (auto h = dynamic_cast<HostileNPC*>(it->get())) {
            if (h->isDead()) {
                // spawn drops
                try {
                    auto &f = game.resources().font("assets/fonts/arial.ttf"); (void)f; // ensure font loaded for potential text if needed
                } catch(...) {}
                // simple RNG
                static std::mt19937 rng(1337u);
                std::uniform_real_distribution<float> dist(0.f,1.f);
                float r1 = dist(rng); float r2 = dist(rng);
                auto hb = h->getBounds(); sf::Vector2f dropPos(hb.position.x + hb.size.x*0.5f, hb.position.y + hb.size.y*0.5f);
                // fiber drop
                if (r1 < 0.6f) {
                    auto fiber = std::make_shared<Item>("fiber", "Plant Fiber", "Common crafting material.", 1);
                    entities.push_back(std::make_unique<ItemEntity>(fiber, dropPos + sf::Vector2f{-4.f,-4.f}));
                }
                // crystal drop
                if (r2 < 0.1f) {
                    auto crystal = std::make_shared<Item>("crystal_raw", "Raw Crystal", "Faintly humming shard used in rituals.", 1);
                    entities.push_back(std::make_unique<ItemEntity>(crystal, dropPos + sf::Vector2f{4.f,4.f}));
                }
                erase = true;
            }
        }
        if (erase) it = entities.erase(it); else ++it;
    }

    // update combat texts
    for (auto &ct : combatTexts) {
        float s = dt.asSeconds();
        ct.text.move(ct.vel * s);
        ct.lifetime -= s;
        sf::Color c = ct.text.getFillColor();
        if (ct.lifetime < 0.4f) { c.a = static_cast<uint8_t>(std::max(0.f, 255.f * (ct.lifetime/0.4f))); ct.text.setFillColor(c); }
    }
    combatTexts.erase(std::remove_if(combatTexts.begin(), combatTexts.end(), [](const CombatText& ct){ return ct.lifetime <= 0.f; }), combatTexts.end());

    // reclaim finished crops (harvested or withered) and restore soil tile to Plantable
    for (auto it = entities.begin(); it != entities.end(); ) {
        if (auto c = dynamic_cast<Crop*>(it->get())) {
            if (c->isFinished()) {
                if (c->wasHarvested()) {
                    harvestedCropsCount++;
                    if (!fertilizerUnlocked && harvestedCropsCount >= 10) { fertilizerUnlocked = true; std::cerr << "Fertilizer unlocked after harvesting 10 crops!\n"; }
                    for (auto &d : directives) if (d.id=="harvest_crops" && !d.satisfied) { d.progress++; }
                    // Spawn HarvestFX
                    sf::FloatRect b = c->getBounds();
                    HarvestFX fx; fx.pos = { b.position.x + b.size.x*0.5f, b.position.y + b.size.y*0.5f }; fx.yield = c->yieldAmount(); fx.duration = 0.45f; harvestFxList.push_back(fx);
                }
                sf::FloatRect b = c->getBounds();
                unsigned tx = (unsigned)std::floor((b.position.x + b.size.x*0.5f)/ map.tileSize());
                unsigned ty = (unsigned)std::floor((b.position.y + b.size.y*0.5f)/ map.tileSize());
                if (tx < map.width() && ty < map.height()) map.setTile(tx,ty, TileMap::Plantable);
                it = entities.erase(it);
                continue;
            }
        }
        ++it;
    }
    // Update HarvestFX animations
    for (auto &fx : harvestFxList) {
        if (!fx.active) continue; fx.elapsed += dt.asSeconds();
        float t = fx.elapsed;
        if (t >= fx.duration) { fx.active = false; continue; }
        // phase mapping: 0 squash 0-0.09, 1 pop 0.09-0.18, 2 fade 0.18-0.26, 3 magnetize tail 0.26-0.45
        if (t < 0.09f) fx.phase = 0; else if (t < 0.18f) fx.phase = 1; else if (t < 0.26f) fx.phase = 2; else fx.phase = 3;
    }
    // prune inactive
    harvestFxList.erase(std::remove_if(harvestFxList.begin(), harvestFxList.end(), [](const HarvestFX& f){ return !f.active; }), harvestFxList.end());

    // mouse click interaction — point-in-rect
    bool leftClick = game.input().wasMousePressed(sf::Mouse::Button::Left);
    bool rightClick = game.input().wasMousePressed(sf::Mouse::Button::Right);
    sf::Vector2i pixelPos = sf::Mouse::getPosition(game.getWindow());
    sf::Vector2f worldPos = game.getWindow().mapPixelToCoords(pixelPos, view);
    // NOTE: use player's cached interact flag (Player::update already consumed actionPressed)
    bool interactPressed = player->wantsToInteract();
    bool plantedThisFrame = false;

    // cart route editing interactions
    if (cartRouteMode && activeCart) {
        unsigned ts = map.tileSize();
        unsigned tx = (unsigned)std::floor(worldPos.x / ts);
        unsigned ty = (unsigned)std::floor(worldPos.y / ts);
        if (leftClick) {
            if (loaderMode) { loaderTile = {tx,ty}; loaderMode = false; }
            else if (unloaderMode) { unloaderTile = {tx,ty}; unloaderMode = false; }
            else if (tx < map.width() && ty < map.height() && map.isTileRail(tx,ty)) {
                activeCart->addWaypoint({tx,ty});
            }
        }
        if (rightClick) {
            activeCart->clearWaypoints();
        }
    } else {
        // existing watering / interaction path
        // simple watering: right-click increases moisture on hovered tile
        if (rightClick) {
            unsigned ts = map.tileSize();
            unsigned tx = (unsigned)std::floor(worldPos.x / ts);
            unsigned ty = (unsigned)std::floor(worldPos.y / ts);
            if (player->hasWateringTool()) {
                // boosted watering if tool present
                map.addWater(tx,ty,0.4f);
                // minor splash to von neumann neighbors
                if (tx>0) map.addWater(tx-1,ty,0.1f);
                if (tx+1<map.width()) map.addWater(tx+1,ty,0.1f);
                if (ty>0) map.addWater(tx,ty-1,0.1f);
                if (ty+1<map.height()) map.addWater(tx,ty+1,0.1f);
            } else if (map.isTilePlantable(tx,ty)) map.addWater(tx,ty,0.25f);
        }
        // planting / interaction: Interact key now prioritizes nearby cart/entity, not mouse hover
        if (interactPressed) {
            // Determine if player is already riding
            bool playerRidingNow = false; Cart* ridingCart = nullptr;
            for (auto &c : carts) if (c->getRider() == player.get()) { playerRidingNow = true; ridingCart = c.get(); break; }
            // Player bounds for proximity checks
            sf::FloatRect pb = player->getBounds();
            const float proxExpand = 12.f; // allow small tolerance
            pb.position.x -= proxExpand; pb.position.y -= proxExpand; pb.size.x += proxExpand*2.f; pb.size.y += proxExpand*2.f;
            auto overlaps = [](const sf::FloatRect& A, const sf::FloatRect& B){
                return !(A.position.x + A.size.x < B.position.x ||
                         B.position.x + B.size.x < A.position.x ||
                         A.position.y + A.size.y < B.position.y ||
                         B.position.y + B.size.y < A.position.y);
            };
            bool handled = false;
            if (playerRidingNow && ridingCart) { ridingCart->dismount(); handled = true; }
            else {
                for (auto &c : carts) { if (overlaps(pb, c->getBounds())) { c->interact(player.get()); handled = true; break; } }
                if (!handled) {
                    for (auto &e : entities) { if (overlaps(pb, e->getBounds())) { e->interact(player.get()); handled = true; break; } }
                }
            }
            if (!handled) { attemptPlanting(worldPos); }
            // reset interact so we don't process again this frame
            player->resetInteract();
        }
        // fertilize: press F to boost fertility on player tile (prototype)
        if (game.input().actionPressed("Fertilize")) {
            if (!fertilizerUnlocked) {
                if (harvestedCropsCount >= 10) fertilizerUnlocked = true; else {
                    std::cerr << "Fertilizer locked: harvest " << (10 - harvestedCropsCount) << " more crops.\n";
                }
            }
            if (fertilizerUnlocked) {
                unsigned ts = map.tileSize();
                sf::Vector2f ppos = player->position();
                unsigned tx = (unsigned)std::floor(ppos.x / ts);
                unsigned ty = (unsigned)std::floor(ppos.y / ts);
                bool usedFert = false;
                for (auto &it : player->inventory().items()) {
                    if (it && it->id == "fert_basic" && it->stackSize > 0) { it->stackSize -= 1; usedFert = true; map.addFertility(tx,ty,0.15f); break; }
                }
                if (!usedFert) map.addFertility(tx,ty,0.05f);
            }
        }
    }

    // update rail tool if enabled
    if (railTool && railTool->enabled) {
        railTool->update(worldPos, leftClick);
        if (leftClick) { syncRailsWithMap(); for (auto &d : directives) if (d.id=="build_rail" && !d.satisfied) { d.progress = 1; } }
    } else {
        if (leftClick) {
            for (size_t i = 0; i < entities.size(); ++i) {
                auto &e = entities[i];
                if (e->getBounds().contains(worldPos)) {
                    if (auto npc = dynamic_cast<NPC*>(e.get())) {
                        dialog.start({ "Hello stranger.", "Nice weather today, isn't it?", "Press E or Space to continue." });
                    } else {
                        e->interact(player.get());
                    }
                }
            }
        }
    }
    // planting directive increment (if planting occurred this frame, detect new crop near player by scanning last entity) - simplistic heuristic
    // (Could be improved with explicit event in future.)
    if (leftClick && !directives.empty()) {
        if (auto it = std::find_if(directives.begin(), directives.end(), [](const Directive& d){return d.id=="plant_seed";}); it!=directives.end() && !it->satisfied) {
            if (entities.size() > lastEntityCount) {
                it->progress = it->target; // force complete
                std::cerr << "Directive completed (plant seed): " << it->text << "\n";
            }
        }
    }
    lastEntityCount = entities.size();

    updateQuests();
    evaluateDirectives();
    updateQuestChain();
    game.input().clearFrame();
}

void PlayState::attemptPlanting(const sf::Vector2f& worldPos) {
    if (!player) return;
    auto &items = player->inventory().itemsMutable();
    int seedIndex = -1; for (size_t i=0;i<items.size();++i) if (items[i] && items[i]->id.rfind("seed_",0)==0 && items[i]->stackSize>0) { seedIndex = (int)i; break; }
    if (seedIndex < 0) return;
    unsigned ts = map.tileSize();
    unsigned tx = (unsigned)std::floor(worldPos.x / ts);
    unsigned ty = (unsigned)std::floor(worldPos.y / ts);
    if (tx >= map.width() || ty >= map.height()) return;
    if (!map.isTilePlantable(tx,ty)) return;
    items[seedIndex]->stackSize -= 1; if (items[seedIndex]->stackSize <= 0) { items.erase(items.begin()+seedIndex); }
    sf::Vector2f pos(tx*ts + ts*0.5f, ty*ts + ts*0.5f);
    entities.push_back(std::make_unique<Crop>(game.resources(), map, pos, "wheat", 3, 6.f));
    map.setTile(tx,ty, TileMap::Empty);
    for (auto &d : directives) if (d.id=="plant_seed" && !d.satisfied) { d.progress = d.target; }
}

void PlayState::clampViewCenter() {
    sf::Vector2f center = view.getCenter();
    sf::Vector2f half = view.getSize()*0.5f;
    sf::Vector2f world = map.worldSize();
    if (center.x - half.x < 0.f) center.x = half.x;
    if (center.y - half.y < 0.f) center.y = half.y;
    if (center.x + half.x > world.x) center.x = world.x - half.x;
    if (center.y + half.y > world.y) center.y = world.y - half.y;
    view.setCenter(center);
}

void PlayState::updateQuestChain() {
    // Simple linear chain driven by directive completion.
    // Stage 0: waiting for plant_seed completion
    // Stage 1: waiting for harvest_crops completion
    // Stage 2: waiting for build_rail completion
    // Stage 3: done
    auto getDir = [&](const std::string& id)->Directive*{ for(auto &d:directives) if(d.id==id) return &d; return nullptr; };
    if (questChainStage == 0) {
        if (auto d = getDir("plant_seed"); d && d->satisfied) {
            questChainStage = 1;
            dialog.start({"Great! You've planted a seed.", "Next: Harvest 5 crops to learn growth cycles."});
        }
    } else if (questChainStage == 1) {
        if (auto d = getDir("harvest_crops"); d && d->satisfied) {
            questChainStage = 2;
            dialog.start({"Harvest milestone reached!", "Now place a rail segment to begin automation."});
        }
    } else if (questChainStage == 2) {
        if (auto d = getDir("build_rail"); d && d->satisfied) {
            questChainStage = 3;
            dialog.start({"First rail placed!", "Directive chain complete."});
        }
    }
}

void PlayState::evaluateDirectives() {
    bool plantDone=false; bool harvestDone=false;
    for (auto &d : directives) {
        if (d.id == "plant_seed") {
            if (d.progress >= d.target && !d.satisfied) { d.progress = d.target; d.satisfied = true; d.completedAt = hudTime; plantDone = true; }
            else if (d.satisfied) plantDone = true;
        } else if (d.id == "harvest_crops") {
            if (d.progress >= d.target && !d.satisfied) { d.progress = d.target; d.satisfied = true; d.completedAt = hudTime; harvestDone = true; }
            else if (d.satisfied) harvestDone = true;
        } else if (d.id == "build_rail") {
            if (d.progress >= d.target && !d.satisfied) { d.progress = d.target; d.satisfied = true; d.completedAt = hudTime; }
        }
    }
    for (auto &d : directives) {
        if (d.id == "harvest_crops") d.hidden = !plantDone && !d.satisfied;
        if (d.id == "build_rail") d.hidden = !harvestDone && !d.satisfied;
        if (d.progress > d.target) d.progress = d.target;
    }
}

void PlayState::draw() {
    sf::RenderWindow &win = game.getWindow();
    win.setView(view);
    // screen shake placeholder using active HarvestFX (minor vertical jitter during pop phase)
    float shakeOffset = 0.f;
    for (auto &fx : harvestFxList) if (fx.phase==1) { shakeOffset = std::max(shakeOffset, 3.f); }
    if (shakeOffset>0.f) view.move(0.f, std::sin(hudTime*40.f)*shakeOffset*0.2f);
    map.draw(win, showRailOverlay);
    for (auto &e : entities) e->draw(win);
    for (auto &c : carts) c->draw(win);
    if (player) player->draw(win);
    for (auto &p : worldProjectiles) p->draw(win);
    // draw HarvestFX
    for (auto &fx : harvestFxList) {
        float t = fx.elapsed;
        float alpha = 1.f;
        float scale = 1.f;
        if (fx.phase==0) { // squash
            scale = 0.6f + 0.4f * (t/0.09f);
        } else if (fx.phase==1) { // pop
            scale = 1.0f + 0.5f * ((t-0.09f)/0.09f);
        } else if (fx.phase==2) { // fade+shrink
            float lt = (t-0.18f)/0.08f; scale = 1.5f - 0.4f*lt; alpha = 1.f - lt;
        } else { // magnet tail (float upward, fade slightly)
            float lt = (t-0.26f)/(fx.duration-0.26f); scale = 1.1f - 0.5f*lt; alpha = 0.6f - 0.6f*lt; fx.pos.y -= 30.f * (dt.asSeconds());
        }
        sf::CircleShape circ(10.f);
        circ.setOrigin({10.f,10.f});
        circ.setPosition(fx.pos);
        circ.setScale({scale, scale});
        circ.setFillColor(sf::Color(255, 230, 80, (uint8_t)std::clamp(alpha*255.f,0.f,255.f)));
        win.draw(circ);
        // yield text popup on first frame of pop
        if (fx.phase==1) {
            try {
                auto &f = game.resources().font("assets/fonts/arial.ttf");
                sf::Text txt(f, "+" + std::to_string(fx.yield), 12u);
                txt.setPosition(fx.pos + sf::Vector2f{-6.f, -16.f});
                txt.setFillColor(sf::Color(255,255,255,200));
                win.draw(txt);
            } catch(...) {}
        }
    }
    if (moistureOverlay) map.drawMoistureOverlay(win);
    if (fertilityOverlay) map.drawFertilityOverlay(win);
    // cart route editing overlay
    if (cartRouteMode && activeCart) {
        const auto &wps = activeCart->getWaypoints();
        float ts = (float)map.tileSize();
        if (wps.size() >= 2) {
            sf::VertexArray lines(sf::PrimitiveType::LineStrip, wps.size());
            for (size_t i=0;i<wps.size();++i) {
                lines[i].position = { wps[i].x*ts + ts*0.5f, wps[i].y*ts + ts*0.5f };
                lines[i].color = sf::Color(0,180,255);
            }
            win.draw(lines);
        }
        if (wps.size() >= 3 && activeCart->isLoop()) {
            sf::Vertex loopLine[2];
            loopLine[0].position = { wps.back().x*ts + ts*0.5f, wps.back().y*ts + ts*0.5f };
            loopLine[1].position = { wps.front().x*ts + ts*0.5f, wps.front().y*ts + ts*0.5f };
            loopLine[0].color = loopLine[1].color = sf::Color(0,120,180,160);
            win.draw(loopLine, 2, sf::PrimitiveType::Lines);
        }
        for (size_t i=0;i<wps.size();++i) {
            sf::CircleShape circ(ts*0.25f);
            circ.setOrigin({circ.getRadius(), circ.getRadius()});
            circ.setPosition({ wps[i].x*ts + ts*0.5f, wps[i].y*ts + ts*0.5f });
            circ.setFillColor(i==activeCart->currentIndex()? sf::Color(255,180,40) : sf::Color(0,160,255,150));
            win.draw(circ);
        }
        auto drawMarker = [&](sf::Vector2u tile, sf::Color col, const char *label){
            if (tile.x==UINT32_MAX) return; float tsL = (float)map.tileSize();
            sf::RectangleShape r({tsL*0.6f, tsL*0.6f}); r.setOrigin(r.getSize()/2.f);
            r.setPosition({ tile.x*tsL + tsL*0.5f, tile.y*tsL + tsL*0.5f });
            r.setFillColor(col); win.draw(r);
            try {
                auto &f = game.resources().font("assets/fonts/arial.ttf");
                sf::Text t(f,label,12u);
                t.setFillColor(sf::Color::Black);
                t.setPosition(r.getPosition() - sf::Vector2f(6.f,8.f));
                win.draw(t);
            } catch(...) {}
        };
        drawMarker(loaderTile, sf::Color(120,255,120,200), "L");
        drawMarker(unloaderTile, sf::Color(255,120,120,200), "U");
    }

    // switch to default view for HUD/static overlays
    win.setView(win.getDefaultView());
    // --- Minimap (now screen-space) --------------------------------
    if (showMinimap) {
        float tilePix = minimapTilePixel; if (tilePix < 1.f) tilePix = 1.f; if (tilePix>8.f) tilePix = 8.f;
        unsigned mw = map.width(); unsigned mh = map.height();
        float mmW = mw * tilePix; float mmH = mh * tilePix;
        // position top-right with padding
        float pad = 8.f;
        sf::Vector2f origin(win.getSize().x - mmW - pad, pad);
        // background panel
        sf::RectangleShape bg({mmW+4.f, mmH+4.f}); bg.setPosition(origin - sf::Vector2f{2.f,2.f}); bg.setFillColor(sf::Color(20,20,28,180)); bg.setOutlineColor(sf::Color(60,60,80)); bg.setOutlineThickness(1.f); win.draw(bg);
        // tiles
        sf::RectangleShape cell({tilePix, tilePix});
        for (unsigned ty=0; ty<mh; ++ty) {
            for (unsigned tx=0; tx<mw; ++tx) {
                if (!map.isExplored(tx,ty)) continue; // fog hidden
                auto t = map.getTile(tx,ty);
                sf::Color c;
                switch(t) {
                    case TileMap::Empty: c = sf::Color(70,110,80); break;
                    case TileMap::Solid: c = sf::Color(50,50,55); break;
                    case TileMap::Plantable: c = sf::Color(110,85,40); break;
                    case TileMap::Rail: c = sf::Color(160,140,80); break;
                }
                cell.setFillColor(c);
                cell.setPosition(origin + sf::Vector2f(tx*tilePix, ty*tilePix));
                win.draw(cell);
            }
        }
        // entities (optional)
        if (showMinimapEntities) {
            sf::RectangleShape dot({tilePix, tilePix});
            dot.setFillColor(sf::Color::Red);
            if (player) {
                sf::Vector2f p = player->position();
                unsigned ts = map.tileSize();
                unsigned px = (unsigned)std::floor(p.x / ts);
                unsigned py = (unsigned)std::floor(p.y / ts);
                if (px < mw && py < mh) { dot.setPosition(origin + sf::Vector2f(px*tilePix, py*tilePix)); win.draw(dot); }
            }
            dot.setFillColor(sf::Color(220,180,60));
            for (auto &e : entities) {
                if (dynamic_cast<HostileNPC*>(e.get())) {
                    sf::FloatRect b = e->getBounds(); unsigned ts = map.tileSize();
                    unsigned ex = (unsigned)std::floor((b.position.x + b.size.x*0.5f)/ts);
                    unsigned ey = (unsigned)std::floor((b.position.y + b.size.y*0.5f)/ts);
                    if (ex < mw && ey < mh && map.isExplored(ex,ey)) { dot.setPosition(origin + sf::Vector2f(ex*tilePix, ey*tilePix)); win.draw(dot); }
                }
            }
            dot.setFillColor(sf::Color(0,160,255));
            for (auto &c : carts) {
                sf::Vector2f cp = c->worldPosition(); unsigned ts = map.tileSize();
                unsigned cx = (unsigned)std::floor(cp.x / ts); unsigned cy = (unsigned)std::floor(cp.y / ts);
                if (cx < mw && cy < mh && map.isExplored(cx,cy)) { dot.setPosition(origin + sf::Vector2f(cx*tilePix, cy*tilePix)); win.draw(dot); }
            }
        }
        // view rectangle
        if (showMinimapViewRect) {
            sf::Vector2f vc = view.getCenter(); sf::Vector2f vs = view.getSize();
            unsigned ts = map.tileSize();
            float left = (vc.x - vs.x*0.5f) / ts; float top = (vc.y - vs.y*0.5f) / ts;
            float right = (vc.x + vs.x*0.5f) / ts; float bottom = (vc.y + vs.y*0.5f) / ts;
            if (right < 0 || bottom < 0 || left > mw || top > mh) { /* ignore */ }
            else {
                if (left < 0) left = 0; if (top < 0) top = 0; if (right > mw) right = (float)mw; if (bottom > mh) bottom = (float)mh;
                sf::RectangleShape vr({(right-left)*tilePix, (bottom-top)*tilePix});
                vr.setPosition(origin + sf::Vector2f(left*tilePix, top*tilePix));
                vr.setFillColor(sf::Color(0,0,0,0)); vr.setOutlineColor(sf::Color(255,255,255,100)); vr.setOutlineThickness(1.f);
                win.draw(vr);
            }
        }
    }
    // ----------------------------------------------------------------

    if (inventoryUI) inventoryUI->draw(win);
    dialog.draw(win);
    auto &f = game.resources().font("assets/fonts/arial.ttf");
    // Player health bar (simple)
    if (player) {
        float hp = player->getHealth(); float maxhp = std::max(1.f, player->getMaxHealth());
        float w = 160.f; float h = 14.f; float pad = 8.f;
        sf::RectangleShape bg({w,h}); bg.setPosition({pad, (float)win.getSize().y - h - pad}); bg.setFillColor(sf::Color(40,40,50,200)); bg.setOutlineThickness(1.f); bg.setOutlineColor(sf::Color(80,80,100));
        win.draw(bg);
        float pct = hp / maxhp; if (pct < 0.f) pct = 0.f; if (pct>1.f) pct=1.f;
        sf::RectangleShape fg({(w-2.f)*pct, h-2.f}); fg.setPosition(bg.getPosition()+sf::Vector2f{1.f,1.f});
        sf::Color col = (pct>0.5f? sf::Color(90,200,90): (pct>0.25f? sf::Color(230,180,60): sf::Color(220,70,50)));
        fg.setFillColor(col); win.draw(fg);
        std::ostringstream ss; ss << (int)hp << "/" << (int)maxhp;
        sf::Text hpTxt(f, ss.str(), 12u); hpTxt.setPosition(bg.getPosition()+sf::Vector2f{4.f,-14.f}); hpTxt.setFillColor(sf::Color::White); win.draw(hpTxt);
    }

    // quest & directives HUD (top-left)
    float y = 8.f;
    if (questChainStage < 3) {
        static const char* stageMsg[] = {"Chain: Plant a seed","Chain: Harvest 5 crops","Chain: Place a rail segment"};
        sf::Text hint(f, stageMsg[questChainStage], 12u); hint.setPosition({8.f,y}); hint.setFillColor(sf::Color(180,255,180)); win.draw(hint); y += 16.f;
    }
    for (auto &d : directives) if (!d.hidden) {
        bool show = true;
        if (d.satisfied) {
            float elapsed = hudTime - d.completedAt; const float displayFor = 3.f; const float fadeFor = 1.5f; // show then fade
            if (elapsed > displayFor + fadeFor) show = false;
            else if (elapsed > displayFor) {
                float alpha = 1.f - (elapsed - displayFor) / fadeFor; if (alpha < 0.f) alpha = 0.f;
                sf::Text dt(f, std::string("[Done] ") + d.text, 12u); dt.setPosition({8.f,y}); dt.setFillColor(sf::Color(120,200,120,(uint8_t)(alpha*255))); win.draw(dt); if (show) y += 14.f; continue;
            }
        }
        if (!show) continue;
        std::string label = (d.satisfied? "[Done] " : "[!] ") + d.text;
        if (d.target>1) label += " ("+std::to_string(d.progress)+"/"+std::to_string(d.target)+")";
        sf::Text dt(f, label, 12u); dt.setPosition({8.f,y}); dt.setFillColor(d.satisfied? sf::Color(120,200,120): sf::Color(255,220,120)); win.draw(dt); y += 14.f;
    }
    if (!directives.empty()) y += 4.f;
    for (auto &q : activeQuests) {
        sf::Text title(f, q.title + (q.completed? " (Done)" : ""), 14u); title.setPosition({8.f,y}); title.setFillColor(sf::Color::White); win.draw(title); y += 16.f;
        for (auto &o : q.objectives) {
            std::ostringstream ss; ss << "  - " << o.id << ": " << o.progress << "/" << o.target; if (o.completed) ss << " ✅";
            sf::Text ot(f, ss.str(), 12u); ot.setPosition({8.f,y}); ot.setFillColor(sf::Color(200,200,200)); win.draw(ot); y += 14.f;
        }
    }

    // Journal panel
    if (showJournal) {
        sf::RectangleShape panel({300.f, (float)win.getSize().y - 40.f}); panel.setPosition({win.getSize().x - 310.f,20.f}); panel.setFillColor(sf::Color(20,20,30,200)); win.draw(panel);
        float jy = 30.f; float jx = win.getSize().x - 300.f;
        sf::Text hdr(f, "Journal", 16u); hdr.setPosition({jx,jy}); hdr.setFillColor(sf::Color::White); win.draw(hdr); jy += 24.f;
        sf::Text dh(f, "Directives", 14u); dh.setPosition({jx,jy}); dh.setFillColor(sf::Color(255,220,120)); win.draw(dh); jy += 18.f;
        for (auto &d : directives) {
            std::string line = (d.satisfied? "[Done] " : "[ ] ") + d.text; if (d.target>1) line += " (" + std::to_string(d.progress) + "/" + std::to_string(d.target) + ")";
            sf::Text dt(f,line,12u); dt.setPosition({jx,jy}); dt.setFillColor(d.satisfied? sf::Color(120,200,120): sf::Color(200,200,200)); win.draw(dt); jy += 14.f;
        }
        jy += 10.f; sf::Text qh(f, "Quests", 14u); qh.setPosition({jx,jy}); qh.setFillColor(sf::Color(180,220,255)); win.draw(qh); jy += 18.f;
        for (auto &q : activeQuests) {
            sf::Text qt(f, q.title, 12u); qt.setPosition({jx,jy}); qt.setFillColor(q.completed? sf::Color(120,200,120): sf::Color::White); win.draw(qt); jy += 14.f;
            for (auto &o : q.objectives) {
                std::ostringstream ss; ss << "   - " << o.id << ": " << o.progress << "/" << o.target; if (o.completed) ss << " ✔";
                sf::Text qo(f, ss.str(), 11u); qo.setPosition({jx,jy}); qo.setFillColor(o.completed? sf::Color(100,180,100): sf::Color(160,160,160)); win.draw(qo); jy += 12.f;
            }
            jy += 6.f;
        }
    }
}

// Restored implementations (previously truncated) ---------------------------------
void PlayState::saveGame(const std::string& path) {
    try {
        nlohmann::json j;
        if (player) {
            sf::Vector2f p = player->position();
            j["player"]["x"] = p.x;
            j["player"]["y"] = p.y;
            j["player"]["health"] = player->getHealth();
        }
        j["quest_chain_stage"] = questChainStage;
        // directives
        for (auto &d : directives) {
            nlohmann::json dj; dj["id"]=d.id; dj["text"]=d.text; dj["satisfied"]=d.satisfied; dj["hidden"]=d.hidden; dj["progress"]=d.progress; dj["target"]=d.target; j["directives"].push_back(dj);
        }
        std::ofstream ofs(path);
        if (ofs) ofs << j.dump(2);
        std::cerr << "Saved game to " << path << "\n";
    } catch(const std::exception& e) {
        std::cerr << "saveGame error: " << e.what() << "\n";
    }
}

void PlayState::loadGame(const std::string& path) {
    try {
        std::ifstream ifs(path);
        if (!ifs) { std::cerr << "loadGame: file not found\n"; return; }
        nlohmann::json j; ifs >> j;
        if (j.contains("player")) {
            auto &pj = j["player"]; if (player) {
                if (pj.contains("x") && pj.contains("y")) player->setPosition({pj["x"].get<float>(), pj["y"].get<float>()});
                if (pj.contains("health")) player->setHealth(pj["health"].get<float>());
            }
        }
        directives.clear();
        if (j.contains("directives") && j["directives"].is_array()) {
            for (auto &dj : j["directives"]) {
                Directive d; if (dj.contains("id")) d.id = dj["id"].get<std::string>();
                if (dj.contains("text")) d.text = dj["text"].get<std::string>();
                if (dj.contains("satisfied")) d.satisfied = dj["satisfied"].get<bool>();
                if (dj.contains("hidden")) d.hidden = dj["hidden"].get<bool>();
                if (dj.contains("progress")) d.progress = dj["progress"].get<int>();
                if (dj.contains("target")) d.target = dj["target"].get<int>();
                // fallback text
                if (d.text.empty()) {
                    if (d.id=="plant_seed") d.text="Plant a seed"; else if (d.id=="harvest_crops") d.text="Harvest 5 crops"; else if (d.id=="build_rail") d.text="Place a rail segment";
                }
                directives.push_back(d);
            }
        } else {
            directives.push_back({"plant_seed","Plant a seed",false,false,0,1});
            directives.push_back({"harvest_crops","Harvest 5 crops",false,true,0,5});
            directives.push_back({"build_rail","Place a rail segment",false,true,0,1});
        }
        if (j.contains("quest_chain_stage")) questChainStage = j["quest_chain_stage"].get<int>();
        evaluateDirectives();
        std::cerr << "Loaded game from " << path << "\n";
    } catch(const std::exception& e) {
        std::cerr << "loadGame error: " << e.what() << "\n";
    }
}

void PlayState::syncRailsWithMap() {
    // remove existing Rail entities
    entities.erase(std::remove_if(entities.begin(), entities.end(), [](const std::unique_ptr<Entity>& e){ return dynamic_cast<Rail*>(e.get()) != nullptr; }), entities.end());
    // recreate from map tiles
    for (unsigned y = 0; y < map.height(); ++y) {
        for (unsigned x = 0; x < map.width(); ++x) {
            if (map.isTileRail(x,y)) {
                sf::Vector2f pos((float)x * map.tileSize(), (float)y * map.tileSize());
                entities.push_back(std::make_unique<Rail>(game.resources(), pos, map.tileSize()));
            }
        }
    }
}

void PlayState::spawnProjectile(std::unique_ptr<Entity> p) {
    if (p) worldProjectiles.push_back(std::move(p));
}

HostileNPC* PlayState::spawnHostile(const sf::Vector2f& pos) {
    float roll = rand01();
    HostileNPC::Type t = (roll < tankSpawnChance ? HostileNPC::Tank : HostileNPC::Grunt);
    auto hostile = std::make_unique<HostileNPC>(pos, player.get(), t);
    hostile->setTileMap(&map);
    HostileNPC* ptr = hostile.get();
    entities.push_back(std::move(hostile));
    return ptr;
}

void PlayState::updateQuests() {
    for (auto &q : activeQuests) {
        if (!q.completed) {
            bool all = true; for (auto &o : q.objectives) if (!o.completed) { all = false; break; }
            if (all) { q.completed = true; std::cerr << "Quest completed: " << q.title << "\n"; }
        }
    }
}
// -------------------------------------------------------------------------------