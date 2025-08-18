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
    } catch (...) {}

    player = std::make_unique<Player>(game.input());
    respawnPos = player->position();
    // give the player some sample seeds for testing — start with more seeds for reliable testing
    for (int i = 0; i < 5; ++i) {
        player->inventory().addItem(std::make_shared<Item>("seed_wheat", "Wheat Seed", "Seed", 1, "assets/textures/items/seed_wheat.png"));
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

    // toggle inventory UI
    if (game.input().wasKeyPressed(sf::Keyboard::Key::I)) {
        if (inventoryUI) inventoryUI->toggle();
        std::cerr << "Inventory UI toggled. open=" << (inventoryUI ? "?" : "no-ui") << "\n";
    }

    // toggle moisture overlay
    if (game.input().wasKeyPressed(sf::Keyboard::Key::M)) {
        moistureOverlay = !moistureOverlay;
        std::cerr << "Moisture overlay " << (moistureOverlay ? "ON" : "OFF") << "\n";
    }
    if (game.input().wasKeyPressed(sf::Keyboard::Key::N)) {
        fertilityOverlay = !fertilityOverlay;
        std::cerr << "Fertility overlay " << (fertilityOverlay ? "ON" : "OFF") << "\n";
    }
    // distance unit toggle (tiles/pixels)
    if (game.input().wasKeyPressed(sf::Keyboard::Key::T)) {
        respawnDistanceInTiles = !respawnDistanceInTiles;
        std::cerr << "Respawn distance units set to " << (respawnDistanceInTiles ? "tiles" : "pixels") << "\n";
    }
    if (game.input().wasKeyPressed(sf::Keyboard::Key::U)) {
        showMinimap = !showMinimap;
        std::cerr << "Minimap " << (showMinimap ? "ON" : "OFF") << "\n";
    }
    // add minimap scale cycle (J)
    if (game.input().wasKeyPressed(sf::Keyboard::Key::J)) {
        // cycle through 2,3,4 then back
        if (minimapTilePixel < 4.f) minimapTilePixel += 1.f; else minimapTilePixel = 2.f;
        std::cerr << "Minimap scale tilePx=" << minimapTilePixel << "\n";
    }
    // toggle minimap view rectangle
    if (game.input().wasKeyPressed(sf::Keyboard::Key::V)) {
        showMinimapViewRect = !showMinimapViewRect;
        std::cerr << "Minimap view rect " << (showMinimapViewRect ? "ON" : "OFF") << "\n";
    }
    // toggle entity icons on minimap
    if (game.input().wasKeyPressed(sf::Keyboard::Key::G)) {
        showMinimapEntities = !showMinimapEntities;
        std::cerr << "Minimap entities " << (showMinimapEntities ? "ON" : "OFF") << "\n";
    }
    if (game.input().wasKeyPressed(sf::Keyboard::Key::H)) {
        showHelpOverlay = !showHelpOverlay;
        std::cerr << "Help overlay " << (showHelpOverlay ? "ON" : "OFF") << "\n";
    }
    // debug key: toggle death penalty mode?
    if (game.input().wasKeyPressed(sf::Keyboard::Key::Y)) {
        enableDeathPenalty = !enableDeathPenalty;
        std::cerr << "Death penalty " << (enableDeathPenalty ? "ENABLED" : "DISABLED") << "\n";
    }
    // quick save/load
    if (game.input().wasKeyPressed(sf::Keyboard::Key::K)) {
        saveGame("SaveGame.json");
    }
    if (game.input().wasKeyPressed(sf::Keyboard::Key::L)) {
        loadGame("SaveGame.json");
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

    timeSinceLastProjectile += dt.asSeconds();
    // fire projectile (space) in facing direction (use movement input velocity as direction) with cooldown
    if (game.input().wasKeyPressed(sf::Keyboard::Key::Space) && timeSinceLastProjectile >= projectileCooldown) {
        sf::Vector2f dir = player->computeDesiredMove(sf::seconds(1.f));
        if (dir.x == 0 && dir.y == 0) dir = {1.f,0.f};
        float len = std::sqrt(dir.x*dir.x + dir.y*dir.y);
        if (len > 0.f) dir /= len;
        spawnProjectile(std::make_unique<Projectile>(player->position(), dir * 300.f));
        timeSinceLastProjectile = 0.f;
    }

    // death handling & penalty
    if (playerDead) {
        respawnTimer += dt.asSeconds();
        if (respawnTimer >= respawnDelay) {
            // optional penalty: reduce inventory item counts or drop percentage
            if (enableDeathPenalty && player) {
                // simple penalty: remove 10% (at least 1 if stack >0) from each stack except seeds
                for (auto &it : player->inventory().items()) {
                    if (!it) continue;
                    if (it->id.rfind("seed_",0)==0) continue; // keep seeds safe
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

    player->updateHealthRegen(dt);

    // compute desired movement and attempt collision-aware moves
    sf::Vector2f desired = player->computeDesiredMove(dt);
    tryMovePlayer(desired);

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

    for (auto& e : entities) e->update(dt);
    // update projectiles
    for (auto &p : worldProjectiles) p->update(dt);
    // projectile lifetime & collision vs hostile NPCs
    for (auto it = worldProjectiles.begin(); it != worldProjectiles.end(); ) {
        bool remove = false;
        if (auto proj = dynamic_cast<Projectile*>(it->get())) {
            if (proj->expired()) remove = true;
            else {
                sf::FloatRect pb = proj->getBounds();
                for (auto &e : entities) {
                    if (auto hostile = dynamic_cast<HostileNPC*>(e.get())) {
                        // replace intersects usage
                        sf::FloatRect hb = hostile->getBounds();
                        sf::FloatRect tb = hb; // rename for clarity
                        if (!(pb.position.x + pb.size.x <= tb.position.x || tb.position.x + tb.size.x <= pb.position.x || pb.position.y + pb.size.y <= tb.position.y || tb.position.y + tb.size.y <= pb.position.y)) {
                            // apply damage
                            hostile->takeDamage(proj->damage);
                            if (hostile->isDead()) {
                                // mark for removal after loop
                            }
                            // spawn combat text
                            try {
                                auto &f = game.resources().font("assets/fonts/arial.ttf");
                                std::ostringstream ss; ss << (int)proj->damage;
                                sf::Text t(f, ss.str(), 14u);
                                t.setFillColor(sf::Color::White);
                                auto hb = hostile->getBounds();
                                t.setPosition({hb.position.x + hb.size.x*0.5f, hb.position.y - 10.f});
                                combatTexts.push_back({t, {0.f, -30.f}, 0.9f});
                            } catch (...) {}
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

    // cull dead hostile NPCs
    entities.erase(std::remove_if(entities.begin(), entities.end(), [](const std::unique_ptr<Entity>& e){ auto h = dynamic_cast<HostileNPC*>(e.get()); return h && h->isDead(); }), entities.end());

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
                // compute tile and mark plantable if inside map
                sf::FloatRect b = c->getBounds();
                unsigned tx = (unsigned)std::floor((b.position.x + b.size.x*0.5f)/ map.tileSize());
                unsigned ty = (unsigned)std::floor((b.position.y + b.size.y*0.5f)/ map.tileSize());
                if (tx < map.width() && ty < map.height()) {
                    map.setTile(tx,ty, TileMap::Plantable);
                }
                it = entities.erase(it);
                continue;
            }
        }
        ++it;
    }

    // mouse click interaction — point-in-rect
    bool leftClick = game.input().wasMousePressed(sf::Mouse::Button::Left);
    bool rightClick = game.input().wasMousePressed(sf::Mouse::Button::Right);
    sf::Vector2i pixelPos = sf::Mouse::getPosition(game.getWindow());
    sf::Vector2f worldPos = game.getWindow().mapPixelToCoords(pixelPos, view);

    // simple watering: right-click increases moisture on hovered tile
    if (rightClick) {
        unsigned ts = map.tileSize();
        unsigned tx = (unsigned)std::floor(worldPos.x / ts);
        unsigned ty = (unsigned)std::floor(worldPos.y / ts);
        if (map.isTilePlantable(tx,ty)) map.addWater(tx,ty,0.25f);
    }
    // fertilize: press F to boost fertility on player tile (prototype)
    if (game.input().wasKeyPressed(sf::Keyboard::Key::F)) {
        unsigned ts = map.tileSize();
        sf::Vector2f ppos = player->position();
        unsigned tx = (unsigned)std::floor(ppos.x / ts);
        unsigned ty = (unsigned)std::floor(ppos.y / ts);
        map.addFertility(tx,ty,0.1f);
        std::cerr << "Fertilized tile " << tx << "," << ty << " (+0.1)\n";
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
                    // if altar became or is active, set respawn
                    if (auto altar = dynamic_cast<Altar*>(e.get()); altar && altar->grantsRespawn()) {
                        respawnPos = altar->getBounds().position + altar->getBounds().size * 0.5f;
                        std::cerr << "Respawn point set at altar." << "\n";
                    }
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
                                entities.push_back(std::make_unique<Crop>(game.resources(), map, center, cropId, 3, 6.f));
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
    auto &win = game.getWindow();

    // WORLD VIEW ------------------------------------------------------------
    map.draw(win);
    if (moistureOverlay) map.drawMoistureOverlay(win);
    if (fertilityOverlay) map.drawFertilityOverlay(win);

    // entities & player
    player->draw(win);
    if (player->timeSinceDamage() < 0.25f && !player->isInvulnerable()) {
        sf::RectangleShape flash(player->size());
        flash.setOrigin(player->size()*0.5f);
        flash.setPosition(player->position());
        flash.setFillColor(sf::Color(255,50,50,120));
        win.draw(flash);
    }
    for (auto &e : entities) e->draw(win);
    for (auto &p : worldProjectiles) p->draw(win);
    for (auto &ct : combatTexts) win.draw(ct.text);

    if (railTool && railTool->enabled) railTool->drawPreview(win);
    dialog.draw(win);
    if (inventoryUI) inventoryUI->draw(win);

    // HUD (switch to default view) ------------------------------------------
    {
        sf::View prev = win.getView();
        win.setView(win.getDefaultView());
        float margin = 10.f;
        float barW = 200.f;
        float barH = 14.f;
        sf::Vector2f barPos(margin, margin);
        float health = player ? player->getHealth() : 0.f;
        float maxHealth = player ? player->getMaxHealth() : 1.f;
        float frac = (maxHealth > 0.f) ? std::clamp(health / maxHealth, 0.f, 1.f) : 0.f;
        sf::RectangleShape bg({barW, barH}); bg.setPosition(barPos); bg.setFillColor(sf::Color(30,30,30,200)); bg.setOutlineColor(sf::Color::Black); bg.setOutlineThickness(1.f); win.draw(bg);
        sf::RectangleShape fg({barW * frac, barH}); fg.setPosition(barPos); fg.setFillColor(sf::Color(200,40,40)); win.draw(fg);
        try {
            auto &f = game.resources().font("assets/fonts/arial.ttf");
            sf::Text txt(f, std::to_string((int)health) + " / " + std::to_string((int)maxHealth), 12u);
            txt.setFillColor(sf::Color::White);
            txt.setPosition({barPos.x + barW + 8.f, barPos.y - 2.f});
            win.draw(txt);
        } catch(...) {}
        win.setView(prev);
    }

    // MINIMAP ---------------------------------------------------------------
    if (showMinimap) {
        sf::View prev = win.getView();
        win.setView(win.getDefaultView());
        const unsigned miniW = map.width();
        const unsigned miniH = map.height();
        const float tilePx = minimapTilePixel;
        const float mapW = miniW * tilePx;
        const float mapH = miniH * tilePx;
        const float padding = 10.f;
        sf::Vector2f origin(win.getSize().x - mapW - padding, padding);
        sf::RectangleShape panel({mapW + 4.f, mapH + 4.f});
        panel.setPosition(origin - sf::Vector2f(2.f,2.f));
        panel.setFillColor(sf::Color(0,0,0,140));
        panel.setOutlineColor(sf::Color(40,40,40,160));
        panel.setOutlineThickness(1.f);
        win.draw(panel);
        sf::RectangleShape cell({tilePx, tilePx});
        for (unsigned y=0; y<miniH; ++y) {
            for (unsigned x=0; x<miniW; ++x) {
                if (!map.isExplored(x,y)) cell.setFillColor(sf::Color(10,15,10,200));
                else {
                    switch(map.getTile(x,y)) {
                        case TileMap::Empty: cell.setFillColor(sf::Color(80,120,90,220)); break;
                        case TileMap::Solid: cell.setFillColor(sf::Color(60,60,60,230)); break;
                        case TileMap::Plantable: cell.setFillColor(sf::Color(140,100,60,230)); break;
                        case TileMap::Rail: cell.setFillColor(sf::Color(110,90,50,230)); break;
                    }
                }
                cell.setPosition(origin + sf::Vector2f(x*tilePx, y*tilePx));
                win.draw(cell);
            }
        }
        // camera viewport rectangle
        if (showMinimapViewRect) {
            float ts = (float)map.tileSize();
            sf::Vector2f vc = view.getCenter();
            sf::Vector2f vs = view.getSize();
            sf::Vector2f topLeft(vc.x - vs.x*0.5f, vc.y - vs.y*0.5f);
            float rx = topLeft.x / ts;
            float ry = topLeft.y / ts;
            float rw = vs.x / ts;
            float rh = vs.y / ts;
            if (rx < 0.f) { rw += rx; rx = 0.f; }
            if (ry < 0.f) { rh += ry; ry = 0.f; }
            if (rx + rw > miniW) rw = miniW - rx;
            if (ry + rh > miniH) rh = miniH - ry;
            if (rw > 0.f && rh > 0.f) {
                sf::RectangleShape vr({rw*tilePx, rh*tilePx});
                vr.setPosition(origin + sf::Vector2f(rx*tilePx, ry*tilePx));
                vr.setFillColor(sf::Color(0,0,0,0));
                vr.setOutlineThickness(1.f);
                vr.setOutlineColor(sf::Color(255,255,255,180));
                win.draw(vr);
            }
        }
        // entity icons
        if (showMinimapEntities) {
            float ts = (float)map.tileSize();
            for (auto &e : entities) {
                sf::FloatRect b = e->getBounds();
                sf::Vector2f c(b.position.x + b.size.x*0.5f, b.position.y + b.size.y*0.5f);
                float ex = c.x / ts; float ey = c.y / ts;
                if (ex < 0 || ey < 0 || ex >= miniW || ey >= miniH) continue;
                sf::RectangleShape icon({tilePx, tilePx});
                icon.setPosition(origin + sf::Vector2f(ex*tilePx, ey*tilePx));
                if (dynamic_cast<Crop*>(e.get())) icon.setFillColor(sf::Color(180,220,80,230));
                else if (dynamic_cast<HostileNPC*>(e.get())) icon.setFillColor(sf::Color(220,70,70,230));
                else if (dynamic_cast<NPC*>(e.get())) icon.setFillColor(sf::Color(90,160,255,230));
                else if (dynamic_cast<ItemEntity*>(e.get())) icon.setFillColor(sf::Color(230,230,120,230));
                else if (dynamic_cast<Altar*>(e.get())) icon.setFillColor(sf::Color(160,255,255,230));
                else if (dynamic_cast<HiddenLocation*>(e.get())) icon.setFillColor(sf::Color(200,120,255,230));
                else continue;
                win.draw(icon);
            }
        }
        // player marker
        float ts = (float)map.tileSize();
        sf::Vector2f ppos = player->position();
        float px = ppos.x / ts; float py = ppos.y / ts;
        sf::RectangleShape pmark({tilePx+1.f, tilePx+1.f});
        pmark.setFillColor(sf::Color::White);
        pmark.setPosition(origin + sf::Vector2f(px*tilePx - 0.5f, py*tilePx - 0.5f));
        win.draw(pmark);
        // respawn marker (only if explored)
        if (showRespawnMarker) {
            float rx = respawnPos.x / ts; float ry = respawnPos.y / ts;
            if (map.isExplored((unsigned)rx,(unsigned)ry)) {
                sf::RectangleShape rmark({tilePx+1.f, tilePx+1.f});
                rmark.setFillColor(sf::Color(100,200,255,255));
                rmark.setPosition(origin + sf::Vector2f(rx*tilePx - 0.5f, ry*tilePx - 0.5f));
                win.draw(rmark);
            }
        }
        win.setView(prev);
    }

    // Death overlay ---------------------------------------------------------
    if (playerDead) {
        sf::View prev = win.getView(); win.setView(win.getDefaultView());
        sf::RectangleShape fade({(float)win.getSize().x,(float)win.getSize().y}); fade.setFillColor(sf::Color(0,0,0,150)); win.draw(fade);
        try {
            auto &f = game.resources().font("assets/fonts/arial.ttf");
            sf::Text msg(f, "You Died", 42u); msg.setFillColor(sf::Color(220,60,60));
            sf::FloatRect gb = msg.getGlobalBounds();
            msg.setPosition({win.getSize().x*0.5f - gb.size.x*0.5f, win.getSize().y*0.4f - gb.size.y*0.5f});
            win.draw(msg);
            float remaining = std::max(0.f, respawnDelay - respawnTimer);
            char buf[64]; snprintf(buf,sizeof(buf),"Respawning in %.1f", remaining);
            sf::Text sub(f, buf, 20u); sub.setFillColor(sf::Color::White);
            sf::FloatRect sb = sub.getGlobalBounds();
            sub.setPosition({win.getSize().x*0.5f - sb.size.x*0.5f, msg.getPosition().y + 60.f});
            win.draw(sub);
        } catch(...) {}
        win.setView(prev);
    }

    // Respawn world-space marker -------------------------------------------
    if (showRespawnMarker) {
        respawnMarkerTime += 0.016f; // approximate; could pass dt if stored
        float pulse = 0.5f + 0.5f * std::sin(respawnMarkerTime * 2.5f);
        float baseRadius = 14.f;
        float radius = baseRadius + 4.f * pulse;
        sf::Vector2f ppos = player->position();
        float dx = ppos.x - respawnPos.x; float dy = ppos.y - respawnPos.y; float dist = std::sqrt(dx*dx + dy*dy);
        const float nearThreshold = 96.f;
        bool isNear = dist <= nearThreshold;
        float proximityFactor = isNear ? (1.f - (dist/nearThreshold)) : 0.f;
        uint8_t alpha = static_cast<uint8_t>(120 + 80 * pulse);
        sf::Color farColor(80,180,255, alpha), nearColor(120,255,140, alpha);
        auto lerpColor = [](sf::Color a, sf::Color b, float f){ return sf::Color(
            (uint8_t)(a.r + (b.r-a.r)*f), (uint8_t)(a.g + (b.g-a.g)*f), (uint8_t)(a.b + (b.b-a.b)*f), (uint8_t)(a.a + (b.a-a.a)*f)); };
        sf::Color c = lerpColor(farColor, nearColor, proximityFactor);
        if (isNear) radius += 3.f * proximityFactor;
        sf::CircleShape ring(radius); ring.setOrigin({radius,radius}); ring.setPosition(respawnPos); ring.setFillColor(sf::Color(0,0,0,0)); ring.setOutlineThickness(2.f); ring.setOutlineColor(c); win.draw(ring);
        sf::CircleShape dot(4.f + 2.f * pulse + 1.5f * proximityFactor); dot.setOrigin({dot.getRadius(), dot.getRadius()}); dot.setPosition(respawnPos); dot.setFillColor(c); win.draw(dot);
    }

    // Off-screen respawn arrow & distance ----------------------------------
    {
        sf::Vector2f viewCenter = view.getCenter(); sf::Vector2f halfSize = view.getSize()*0.5f; sf::FloatRect worldView(viewCenter - halfSize, view.getSize()); bool respawnOnScreen = worldView.contains(respawnPos);
        sf::View prev = win.getView(); win.setView(win.getDefaultView());
        if (!respawnOnScreen) {
            sf::Vector2u sz = win.getSize(); sf::Vector2f screenCenter(sz.x*0.5f, sz.y*0.5f);
            sf::Vector2f dir = respawnPos - player->position(); float len = std::sqrt(dir.x*dir.x + dir.y*dir.y); if (len > 0.0001f) dir /= len; else dir = {1.f,0.f};
            float margin = 24.f; sf::Vector2f arrowPos = screenCenter + dir * (std::min(screenCenter.x, screenCenter.y) - margin);
            arrowPos.x = std::clamp(arrowPos.x, margin, (float)sz.x - margin); arrowPos.y = std::clamp(arrowPos.y, margin, (float)sz.y - margin);
            float fadeRange = 600.f; float fadeFactor = std::min(1.f, len / fadeRange); uint8_t arrowAlpha = (uint8_t)(60 + (180-60)*fadeFactor);
            sf::ConvexShape arrow(3); float base=14.f, height=22.f; arrow.setPoint(0,{0.f,-height*0.5f}); arrow.setPoint(1,{base*0.5f,height*0.5f}); arrow.setPoint(2,{-base*0.5f,height*0.5f}); float angle = std::atan2(dir.y, dir.x)*180.f/3.14159265f + 90.f; arrow.setRotation(sf::degrees(angle)); arrow.setPosition(arrowPos); arrow.setFillColor(sf::Color(255,255,255,arrowAlpha)); arrow.setOutlineThickness(2.f); arrow.setOutlineColor(sf::Color(30,30,30,(uint8_t)std::min<int>(200, arrowAlpha+30))); win.draw(arrow);
            if (showRespawnDistance) {
                float distanceWorld = len; float tileSizeF = (float)map.tileSize(); float distanceTiles = tileSizeF>0.f ? distanceWorld / tileSizeF : 0.f; char buf[64];
                try {
                    auto &f = game.resources().font("assets/fonts/arial.ttf");
                    if (respawnDistanceInTiles) {
                        if (distanceTiles < 10.f) snprintf(buf,sizeof(buf),"%.1ft", distanceTiles);
                        else if (distanceTiles < 100.f) snprintf(buf,sizeof(buf),"%.0ft", distanceTiles);
                        else if (distanceTiles >= 1000.f) snprintf(buf,sizeof(buf),"%.1fKt", distanceTiles/1000.f);
                        else snprintf(buf,sizeof(buf),"%.0ft", distanceTiles);
                    } else {
                        if (distanceWorld < 1000.f) snprintf(buf,sizeof(buf),"%.0f", distanceWorld);
                        else if (distanceWorld < 10000.f) snprintf(buf,sizeof(buf),"%.2fK", distanceWorld/1000.f);
                        else snprintf(buf,sizeof(buf),"%.1fK", distanceWorld/1000.f);
                    }
                    sf::Text dtxt(f, buf, 14u); dtxt.setFillColor(sf::Color(255,255,255,arrowAlpha)); sf::FloatRect tb = dtxt.getGlobalBounds(); dtxt.setPosition({arrowPos.x - tb.size.x*0.5f, arrowPos.y + 18.f}); win.draw(dtxt);
                } catch(...) {}
            }
        }
        win.setView(prev);
    }

    // Help Overlay ----------------------------------------------------------
    if (showHelpOverlay) {
        sf::View prev = win.getView(); win.setView(win.getDefaultView());
        try {
            auto &f = game.resources().font("assets/fonts/arial.ttf");
            const char *lines[] = {
                "I Inventory  M Moisture  N Fertility  F Fertilize",
                "E Interact/Plant  Space Shoot  B RailTool  H Help",
                "P RespawnMarker  O RespawnDist  T Units tiles/pix",
                "U Minimap  J MinimapScale  V ViewRect  G Entities",
                "Y DeathPenalty  K Save  L Load"
            };
            float padding = 8.f; float lineH = 16.f; int count = (int)(sizeof(lines)/sizeof(lines[0]));
            float w = 520.f; float h = padding*2 + count * lineH + 4.f;
            sf::RectangleShape bg({w,h}); bg.setPosition({10.f, 60.f}); bg.setFillColor(sf::Color(0,0,0,160)); bg.setOutlineColor(sf::Color(80,80,80,200)); bg.setOutlineThickness(1.f); win.draw(bg);
            for (int i=0;i<count;++i) { sf::Text t(f, lines[i], 14u); t.setFillColor(sf::Color::White); t.setPosition({18.f, 66.f + i*lineH}); win.draw(t);}            
        } catch(...) {}
        win.setView(prev);
    }
}

void PlayState::saveGame(const std::string &path) {
    nlohmann::json j; sf::Vector2f pos = player->position();
    j["player"]["x"] = pos.x; j["player"]["y"] = pos.y; j["player"]["health"] = player->getHealth();
    j["player"]["respawn_x"] = respawnPos.x; j["player"]["respawn_y"] = respawnPos.y; j["player"]["dead"] = playerDead;
    j["player"]["show_respawn_marker"] = showRespawnMarker; j["player"]["show_respawn_distance"] = showRespawnDistance; j["player"]["respawn_distance_tiles"] = respawnDistanceInTiles;
    j["player"]["show_minimap"] = showMinimap; j["player"]["death_penalty"] = enableDeathPenalty; j["player"]["minimap_tile_px"] = minimapTilePixel; j["player"]["minimap_view_rect"] = showMinimapViewRect; j["player"]["minimap_entities"] = showMinimapEntities; j["player"]["help_overlay"] = showHelpOverlay;
    j["player"]["inventory"] = player->inventory().toJson();
    nlohmann::json crops = nlohmann::json::array(); for (auto &e : entities) if (auto c = dynamic_cast<Crop*>(e.get())) crops.push_back(c->toJson()); j["crops"] = crops;
    j["map"] = map.toJson();
    nlohmann::json hostiles = nlohmann::json::array();
    for (auto &e : entities) if (auto h = dynamic_cast<HostileNPC*>(e.get())) { auto b = h->getBounds(); nlohmann::json hj; hj["x"] = b.position.x + b.size.x*0.5f; hj["y"] = b.position.y + b.size.y*0.5f; hj["health"] = h->getHealth(); hostiles.push_back(hj);} j["hostiles"] = hostiles;
    std::ofstream os(path); if (!os) return; os << j.dump(4); std::cerr << "Saved game\n";
}

void PlayState::loadGame(const std::string &path) {
    std::ifstream is(path); if (!is) return; nlohmann::json j; is >> j; if (j.contains("player")) {
        auto &p = j["player"]; if (p.contains("x") && p.contains("y")) player->setPosition({p["x"].get<float>(), p["y"].get<float>()});
        if (p.contains("respawn_x") && p.contains("respawn_y")) { respawnPos.x = p["respawn_x"].get<float>(); respawnPos.y = p["respawn_y"].get<float>(); }
        if (p.contains("inventory")) player->inventory().fromJson(p["inventory"]);
        if (p.contains("health")) { float h = p["health"].get<float>(); player->healToFull(); if (h < player->getMaxHealth()) player->takeDamage(player->getMaxHealth() - h); }
        if (p.contains("dead") && p["dead"].get<bool>()) { playerDead = true; if (!player->isDead()) player->takeDamage(player->getHealth()); respawnTimer = 0.f; } else playerDead = false;
        if (p.contains("show_respawn_marker")) showRespawnMarker = p["show_respawn_marker"].get<bool>();
        if (p.contains("show_respawn_distance")) showRespawnDistance = p["show_respawn_distance"].get<bool>();
        if (p.contains("respawn_distance_tiles")) respawnDistanceInTiles = p["respawn_distance_tiles"].get<bool>();
        if (p.contains("show_minimap")) showMinimap = p["show_minimap"].get<bool>();
        if (p.contains("death_penalty")) enableDeathPenalty = p["death_penalty"].get<bool>();
        if (p.contains("minimap_tile_px")) minimapTilePixel = std::clamp(p["minimap_tile_px"].get<float>(), 1.f, 8.f);
        if (p.contains("minimap_view_rect")) showMinimapViewRect = p["minimap_view_rect"].get<bool>();
        if (p.contains("minimap_entities")) showMinimapEntities = p["minimap_entities"].get<bool>();
        if (p.contains("help_overlay")) showHelpOverlay = p["help_overlay"].get<bool>();
    }
    if (j.contains("crops") && j["crops"].is_array()) { entities.erase(std::remove_if(entities.begin(), entities.end(), [](const std::unique_ptr<Entity>& e){ return dynamic_cast<Crop*>(e.get())!=nullptr;}), entities.end()); for (auto &cj : j["crops"]) if (auto nc = Crop::fromJson(game.resources(), map, cj)) entities.push_back(std::move(nc)); }
    if (j.contains("map")) { map.fromJson(j["map"]); syncRailsWithMap(); }
    if (j.contains("hostiles") && j["hostiles"].is_array()) { entities.erase(std::remove_if(entities.begin(), entities.end(), [](const std::unique_ptr<Entity>& e){ return dynamic_cast<HostileNPC*>(e.get())!=nullptr;}), entities.end()); for (auto &hj : j["hostiles"]) if (hj.contains("x") && hj.contains("y")) { sf::Vector2f hpos(hj["x"].get<float>(), hj["y"].get<float>()); auto hostile = std::make_unique<HostileNPC>(hpos, player.get()); if (hj.contains("health")) hostile->setHealth(hj["health"].get<float>()); entities.push_back(std::move(hostile)); } }
    std::cerr << "Loaded game\n";
}

void PlayState::syncRailsWithMap() {
    // remove existing Rail entities first
    entities.erase(std::remove_if(entities.begin(), entities.end(), [](const std::unique_ptr<Entity>& e){
        return dynamic_cast<Rail*>(e.get()) != nullptr;
    }), entities.end());

    // recreate Rail entities based on tile map
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