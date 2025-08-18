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
#include "../states/HiddenRealmState.h"
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
#include "../entities/Projectile.h"

// rect center for current SFML (uses position/size members)
static sf::Vector2f rect_center(const sf::FloatRect& r) {
    return { r.position.x + r.size.x * 0.5f, r.position.y + r.size.y * 0.5f };
}

PlayState::PlayState(Game& g)
: game(g), view(g.getWindow().getDefaultView()), map(50, 30, 32)
{
    map.generateTestMap();

    // try to set a font for dialog (user should place Arial at assets/fonts/arial.ttf)
    try {
        auto &f = game.resources().font("assets/fonts/arial.ttf");
        dialog.setFont(std::shared_ptr<sf::Font>(&f, [](sf::Font*){}));
    } catch (const std::exception &e) {
        // fallback: try loading from project root (use PROJECT_ROOT defined by CMake)
        try {
            std::string alt = std::string(PROJECT_ROOT) + "/assets/fonts/arial.ttf";
            auto &f = game.resources().font(alt);
            dialog.setFont(std::shared_ptr<sf::Font>(&f, [](sf::Font*){}));
        } catch (const std::exception &e2) {
            // silent: missing font will show no dialog text; user can add font as documented
        }
    }

    player = std::make_unique<Player>(game.input(), game.resources());
    // give the player some sample seeds for testing — start with more seeds for reliable testing
    for (int i = 0; i < 10; ++i) {
        player->inventory().addItem(std::make_shared<Item>("seed_wheat", "Wheat Seed", "Plantable wheat seed", 1, "assets/textures/items/seed_wheat.png"));
    }
    // position player near the soil patch created by generateTestMap() (tile ~6,6)
    unsigned ts = map.tileSize();
    unsigned startTx = 6, startTy = 6;
    player->setPosition({ startTx * (float)ts + ts * 0.5f, startTy * (float)ts + ts * 0.5f });
    std::cerr << "Player positioned near soil at tile " << startTx << "," << startTy << " and given 10 seeds\n";

    // remember respawn position
    respawnPos = player->position();

    // now that player exists, create inventoryUI with player's inventory reference
    inventoryUI = std::make_unique<InventoryUI>(game.resources(), player->inventory());
    entities.push_back(std::make_unique<NPC>(game.resources(), sf::Vector2f(700.f, 380.f)));
    auto sample = std::make_shared<Item>("apple_01", "Apple", "A juicy apple", 1);
    entities.push_back(std::make_unique<ItemEntity>(sample, sf::Vector2f(600.f, 380.f)));

    // spawn crops
    entities.push_back(std::make_unique<Crop>(game.resources(), sf::Vector2f(300.f, 300.f), "wheat", 3, 5.f));
    entities.push_back(std::make_unique<Crop>(game.resources(), sf::Vector2f(340.f, 300.f), "wheat", 3, 7.f));

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
    entities.push_back(std::make_unique<HostileNPC>(game.resources(), sf::Vector2f(400.f, 300.f), player.get()));

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

    // call syncRailsWithMap() to populate initial rail entities
    syncRailsWithMap();
}

void PlayState::handleEvent(const sf::Event& /*ev*/) {
    // Per-frame polling is used in update; events can be forwarded here when needed.
}

bool PlayState::tryMovePlayer(const sf::Vector2f& desired) {
    // axis-aligned resolution: separate X and Y moves
    const sf::Vector2f ppos = player->position();
    const sf::Vector2f psize = player->size();
    const sf::Vector2f half = psize * 0.5f;

    // try X
    sf::Vector2f moveX{desired.x, 0.f};
    sf::FloatRect trialX(
        sf::Vector2f{ppos.x - half.x + moveX.x, ppos.y - half.y},
        sf::Vector2f{psize.x, psize.y}
    );
    if (!map.isRectColliding(trialX)) {
        player->applyMove(moveX);
    }

    // try Y
    sf::Vector2f moveY{0.f, desired.y};
    sf::FloatRect trialY(
        sf::Vector2f{ppos.x - half.x, ppos.y - half.y + moveY.y},
        sf::Vector2f{psize.x, psize.y}
    );
    if (!map.isRectColliding(trialY)) {
        player->applyMove(moveY);
        return true;
    }

    return true;
}

void PlayState::update(sf::Time dt) {
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

    // if player is dead, run respawn timer and skip world updates until respawn
    if (!playerDead && player && player->isDead()) {
        playerDead = true;
        respawnTimer = respawnDelay;
        std::cerr << "Player died — respawning in " << respawnDelay << "s\n";
    }
    if (playerDead) {
        respawnTimer -= dt.asSeconds();
        if (respawnTimer <= 0.f) {
            if (player) {
                player->healToFull();
                player->setPosition(respawnPos);
            }
            playerDead = false;
            std::cerr << "Player respawned at " << respawnPos.x << "," << respawnPos.y << "\n";
        }
        game.input().clearFrame();
        return;
    }

    // toggle inventory UI
    if (game.input().wasKeyPressed(sf::Keyboard::Key::I)) {
        if (inventoryUI) inventoryUI->toggle();
        std::cerr << "Inventory UI toggled. open=" << (inventoryUI ? "?" : "no-ui") << "\n";
    }

    // ensure UI gets per-frame input handling
    if (inventoryUI) inventoryUI->update(game.input(), game.getWindow(), dt);

    // toggle rail build tool
    if (game.input().wasKeyPressed(sf::Keyboard::Key::B)) {
        if (railTool) {
            railTool->toggle();
            std::cerr << "Rail tool toggled. enabled=" << (railTool->enabled ? 1 : 0) << "\n";
        }
    }

    // process input into player (sets vel & flags)
    player->update(dt);

    // transfer any projectiles spawned by player into the world projectiles list
    if (!player->projectiles.empty()) {
        for (auto &pp : player->projectiles) {
            worldProjectiles.push_back(std::move(pp));
        }
        player->projectiles.clear();
    }

    // compute desired movement and attempt collision-aware moves
    sf::Vector2f desired = player->computeDesiredMove(dt);
    tryMovePlayer(desired);

    for (auto& e : entities) e->update(dt);

    // check for activated altars and handle transition to HiddenRealmState
    for (auto &e : entities) {
        if (auto altar = dynamic_cast<Altar*>(e.get())) {
            if (altar->isActive() && !hiddenRealmActive) {
                // confirm activation via dialog
                dialog.start({ "The altar hums with power.", "Do you wish to step through the portal? (Press E)" });
                // mark as pending; once player confirms (E) we will switch states
                hiddenRealmActive = true;
            }
        }
    }

    // if player confirms while awaiting hidden realm, switch states
    if (hiddenRealmActive && game.input().wasKeyPressed(sf::Keyboard::Key::E)) {
        std::cerr << "Player confirmed entering hidden realm. Switching state...\n";
        // push hidden realm state
        game.setState(std::make_unique<HiddenRealmState>(game));
        return; // Stop further world updates; new state takes over
    }

    // update projectiles
    for (auto& p : worldProjectiles) p->update(dt);

    // projectile collisions vs hostile NPCs: projectile hits hostile NPCs
    for (auto& p : worldProjectiles) {
        auto proj = dynamic_cast<Projectile*>(p.get());
        if (!proj) continue;
        sf::FloatRect pb = proj->getBounds();
        for (size_t i = 0; i < entities.size(); ++i) {
            if (auto hn = dynamic_cast<HostileNPC*>(entities[i].get())) {
                sf::FloatRect hb = hn->getBounds();
                // manual AABB overlap test (using position and size members)
                bool overlap = !(pb.position.x + pb.size.x <= hb.position.x ||
                                 hb.position.x + hb.size.x <= pb.position.x ||
                                 pb.position.y + pb.size.y <= hb.position.y ||
                                 hb.position.y + hb.size.y <= pb.position.y);
                if (overlap) {
                    // apply projectile's damage
                    float dmg = proj->getDamage();
                    hn->takeDamage(dmg);
                    // destroy the projectile
                    proj->kill();
                    // if NPC is dead, remove it
                    if (hn->isDead()) {
                        entities.erase(entities.begin() + i);
                    }
                    break;
                }
            }
        }
    }

    // remove expired projectiles if they implement expired() via Projectile
    worldProjectiles.erase(std::remove_if(worldProjectiles.begin(), worldProjectiles.end(), [](const std::unique_ptr<Entity>& e){
        auto pr = dynamic_cast<Projectile*>(e.get());
        return pr && pr->expired();
    }), worldProjectiles.end());

    // mouse click interaction — point-in-rect
    bool leftClick = game.input().wasMousePressed(sf::Mouse::Button::Left);
    sf::Vector2i pixelPos = sf::Mouse::getPosition(game.getWindow());
    sf::Vector2f worldPos = game.getWindow().mapPixelToCoords(pixelPos, view);

    // fire projectile toward mouse when Space is pressed
    if (game.input().wasKeyPressed(sf::Keyboard::Key::Space)) {
        sf::Vector2f dir = worldPos - player->position();
        float len = std::sqrt(dir.x*dir.x + dir.y*dir.y);
        if (len > 1e-6f) {
            dir /= len;
            // spawn projectile from player into world projectiles so collisions are handled
            spawnProjectile(std::make_unique<Projectile>(player->position(), dir * 300.f));
        }
    }

    // update rail tool if enabled
    if (railTool && railTool->enabled) {
        railTool->update(worldPos, leftClick);
        // regenerate rail entities after a placement/removal so world entities reflect tile state
        if (leftClick) syncRailsWithMap();
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

    // E key interaction: proximity-based check
    if (game.input().wasKeyPressed(sf::Keyboard::Key::E)) {
        const sf::Vector2f ppos = player->position();
        const float interactDist = 48.f; // tuneable
        const float interactDistSq = interactDist * interactDist;

        bool didInteract = false;
        for (size_t i = 0; i < entities.size(); ++i) {
            auto &e = entities[i];
            sf::FloatRect eb = e->getBounds();
            sf::Vector2f ecenter = rect_center(eb);

            float dx = ecenter.x - ppos.x;
            float dy = ecenter.y - ppos.y;
            float distSq = dx*dx + dy*dy;
            if (distSq <= interactDistSq) {
                if (auto npc = dynamic_cast<NPC*>(e.get())) {
                    dialog.start({ "You pressed E.", "This NPC responds to proximity." });
                    didInteract = true;
                } else {
                    player->interact(e.get());
                    e->interact(player.get());
                    didInteract = true;
                }
            }
        }

        // if nothing interacted, try planting seeds on nearby soil
        if (!didInteract) {
            std::cerr << "E pressed with no entity interaction: attempting to plant if seed available. Player pos=" << ppos.x << "," << ppos.y << "\n";
            // find a seed in inventory
            std::string foundSeedId;
            for (auto &it : player->inventory().items()) {
                if (it && it->id.rfind("seed_", 0) == 0) {
                    foundSeedId = it->id;
                    break;
                }
            }
            if (!foundSeedId.empty()) {
                std::cerr << "Found seed in inventory: " << foundSeedId << "\n";
                // find a nearby plantable tile (within interactDist)
                unsigned ts = map.tileSize();
                int px = static_cast<int>(std::floor(ppos.x)) / static_cast<int>(ts);
                int py = static_cast<int>(std::floor(ppos.y)) / static_cast<int>(ts);
                std::cerr << "Player tile coords: " << px << "," << py << " (tileSize=" << ts << ")\n";
                bool planted = false;
                for (int oy=-1; oy<=1 && !planted; ++oy) {
                    for (int ox=-1; ox<=1 && !planted; ++ox) {
                        int tx = px + ox;
                        int ty = py + oy;
                        if (tx < 0 || ty < 0 || static_cast<unsigned>(tx) >= map.width() || static_cast<unsigned>(ty) >= map.height()) continue;
                        sf::Vector2f center((float)tx * (float)ts + ts * 0.5f, (float)ty * (float)ts + ts * 0.5f);
                        float dx = center.x - ppos.x;
                        float dy = center.y - ppos.y;
                        float dsq = dx*dx + dy*dy;
                        std::cerr << "Checking tile " << tx << "," << ty << " center=" << center.x << "," << center.y << " distSq=" << dsq << "\n";
                        if (dx*dx + dy*dy > interactDistSq) {
                            std::cerr << "  -> tile too far, skipping\n";
                            continue;
                        }
                        if (map.isTilePlantable((unsigned)tx, (unsigned)ty)) {
                            std::cerr << "  -> tile is plantable\n";
                            // consume one seed and spawn crop
                            if (player->inventory().removeItemById(foundSeedId, 1)) {
                                // seed id has form seed_<cropid>
                                std::string cropId = foundSeedId.substr(std::string("seed_").size());
                                entities.push_back(std::make_unique<Crop>(game.resources(), center, cropId, 3, 6.f));
                                planted = true;
                                std::cerr << "Planted seed " << foundSeedId << " at tile " << tx << "," << ty << "\n";
                                // mark tile as non-plantable (set to Empty) to avoid replanting same spot
                                map.setTile((unsigned)tx, (unsigned)ty, TileMap::Empty);
                            } else {
                                std::cerr << "  -> failed to remove seed from inventory (removeItemById returned false)\n";
                            }
                        } else {
                            std::cerr << "  -> tile not plantable\n";
                        }
                    }
                }
                if (!planted) {
                    std::cerr << "Found seed '" << foundSeedId << "' but no nearby plantable tile within interact range.\n";
                }
            } else {
                std::cerr << "No seed found in player inventory when attempting to plant.\n";
            }
        }
    }

    // Remove collected ItemEntity objects
    entities.erase(std::remove_if(entities.begin(), entities.end(),
        [](const std::unique_ptr<Entity>& e){
            auto it = dynamic_cast<ItemEntity*>(e.get());
            return it && it->collected();
        }), entities.end());

    // Camera follows player, then clamp to map bounds
    view.setCenter(player->position());
    // clamp view center to map extents
    sf::Vector2f halfSize = view.getSize() * 0.5f;
    sf::Vector2f worldSz = map.worldSize();
    sf::Vector2f center = view.getCenter();
    center.x = std::max(halfSize.x, std::min(center.x, worldSz.x - halfSize.x));
    center.y = std::max(halfSize.y, std::min(center.y, worldSz.y - halfSize.y));
    view.setCenter(center);
    game.getWindow().setView(view);

    // per-frame input housekeeping
    game.input().clearFrame();
}

void PlayState::draw() {
    auto& win = game.getWindow();
    // draw map first (world)
    map.draw(win);

    // draw entities & player
    player->draw(win);
    for (auto& e : entities) e->draw(win);

    // draw projectiles
    for (auto& p : worldProjectiles) p->draw(win);

    // draw rail preview if enabled
    if (railTool && railTool->enabled) railTool->drawPreview(win);

    // draw dialog UI on top
    dialog.draw(win);

    // draw inventory UI last
    if (inventoryUI) inventoryUI->draw(win);

    // draw a simple HUD (player health) in screen space using the default view
    {
        // save current view and switch to default (screen) space
        sf::View prev = win.getView();
        win.setView(win.getDefaultView());

        float margin = 10.f;
        float barW = 200.f;
        float barH = 14.f;
        sf::Vector2f barPos(margin, margin);

        float health = player ? player->getHealth() : 0.f;
        float maxHealth = player ? player->getMaxHealth() : 1.f;
        float frac = (maxHealth > 0.f) ? (health / maxHealth) : 0.f;
        frac = std::max(0.f, std::min(1.f, frac));

        sf::RectangleShape bg(sf::Vector2f(barW, barH));
        bg.setPosition(barPos);
        bg.setFillColor(sf::Color(30,30,30,200));
        bg.setOutlineColor(sf::Color::Black);
        bg.setOutlineThickness(1.f);
        win.draw(bg);

        sf::RectangleShape fg(sf::Vector2f(barW * frac, barH));
        fg.setPosition(barPos);
        fg.setFillColor(sf::Color(200,40,40));
        win.draw(fg);

        // optional health text if font available
        try {
            auto &f = game.resources().font("assets/fonts/arial.ttf");
            std::string healthStr = std::to_string((int)health) + " / " + std::to_string((int)maxHealth);
            sf::Text txt(f, healthStr, 12u); // SFML Text requires a Font at construction
            txt.setFillColor(sf::Color::White);
            txt.setPosition(sf::Vector2f(barPos.x + barW + 8.f, barPos.y - 2.f));
            win.draw(txt);
        } catch (...) {
            // font missing: silent
        }

        // restore world view
        win.setView(prev);
    }
}

void PlayState::saveGame(const std::string& path) {
    nlohmann::json j;
    // player pos
    sf::Vector2f pos = player->position();
    j["player"]["x"] = pos.x;
    j["player"]["y"] = pos.y;
    // inventory items (IDs)
    std::vector<std::string> ids;
    for (auto &it : player->inventory().items()) {
        if (it) ids.push_back(it->id);
    }
    j["player"]["inventory"] = ids;

    // tilemap
    j["map"] = map.toJson();

    std::ofstream os(path);
    if (!os) {
        return;
    }
    os << j.dump(4);
    // minimal log kept
    std::cerr << "Saved game\n";
}

void PlayState::loadGame(const std::string& path) {
    std::ifstream is(path);
    if (!is) {
        return;
    }
    nlohmann::json j; is >> j;
    if (j.contains("player")) {
        auto &p = j["player"];
        if (p.contains("x") && p.contains("y")) {
            player->setPosition({p["x"].get<float>(), p["y"].get<float>()});
        }
        if (p.contains("inventory")) {
            // naive: clear inventory and add simple Item entries
            // not implemented: Inventory needs a clear/add by id helper; skip for now
        }
    }

    if (j.contains("map")) {
        map.fromJson(j["map"]);
        syncRailsWithMap();
    }
    std::cerr << "Loaded game\n";
}

void PlayState::syncRailsWithMap() {
    // remove existing Rail entities first
    entities.erase(std::remove_if(entities.begin(), entities.end(), [](const std::unique_ptr<Entity>& e){
        return dynamic_cast<Rail*>(e.get()) != nullptr;
    }), entities.end());

    // create rails for each tile
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