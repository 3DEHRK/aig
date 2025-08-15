#include "PlayState.h"
#include "../core/Game.h"
#include "../resources/ResourceManager.h"
#include "../entities/Player.h"
#include "../entities/NPC.h"
#include "../entities/ItemEntity.h"
#include "../entities/Crop.h"
#include "../entities/Rail.h"
#include "../entities/HostileNPC.h"
#include "../input/InputManager.h"
#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include <algorithm>
#include <cmath>
#include "../systems/SaveGame.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include "../items/Item.h"

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
    // give the player some sample seeds for testing
    player->inventory().addItem(std::make_shared<Item>("seed_wheat", "Wheat Seed", "Plantable wheat seed", 5, "assets/textures/items/seed_wheat.png"));

    // now that player exists, create inventoryUI with player's inventory reference
    inventoryUI = std::make_unique<InventoryUI>(game.resources(), player->inventory());
    entities.push_back(std::make_unique<NPC>(game.resources(), sf::Vector2f(700.f, 380.f)));
    auto sample = std::make_shared<Item>("apple_01", "Apple", "A juicy apple", 1);
    entities.push_back(std::make_unique<ItemEntity>(sample, sf::Vector2f(600.f, 380.f)));

    // spawn crops
    entities.push_back(std::make_unique<Crop>(game.resources(), sf::Vector2f(300.f, 300.f), "wheat", 3, 5.f));
    entities.push_back(std::make_unique<Crop>(game.resources(), sf::Vector2f(340.f, 300.f), "wheat", 3, 7.f));

    // add sample rail pieces in a small area
    entities.push_back(std::make_unique<Rail>(game.resources(), sf::Vector2f(200.f, 200.f)));
    entities.push_back(std::make_unique<Rail>(game.resources(), sf::Vector2f(232.f, 200.f)));
    entities.push_back(std::make_unique<Rail>(game.resources(), sf::Vector2f(264.f, 200.f)));

    // spawn a hostile NPC targeting the player
    entities.push_back(std::make_unique<HostileNPC>(game.resources(), sf::Vector2f(400.f, 300.f), player.get()));
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
        // per-frame input housekeeping
        game.input().clearFrame();
        return;
    }

    // toggle inventory UI
    if (game.input().wasKeyPressed(sf::Keyboard::Key::I)) inventoryUI->toggle();

    // process input into player (sets vel & flags)
    player->update(dt);

    // compute desired movement and attempt collision-aware moves
    sf::Vector2f desired = player->computeDesiredMove(dt);
    tryMovePlayer(desired);

    for (auto& e : entities) e->update(dt);

    // mouse click interaction — point-in-rect
    if (game.input().wasMousePressed(sf::Mouse::Button::Left)) {
        sf::Vector2i pixelPos = sf::Mouse::getPosition(game.getWindow());
        sf::Vector2f worldPos = game.getWindow().mapPixelToCoords(pixelPos, view);
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
            // find a seed in inventory
            std::string foundSeedId;
            for (auto &it : player->inventory().items()) {
                if (it && it->id.rfind("seed_", 0) == 0) {
                    foundSeedId = it->id;
                    break;
                }
            }
            if (!foundSeedId.empty()) {
                // find a nearby plantable tile (within interactDist)
                unsigned ts = map.tileSize();
                int px = static_cast<int>(std::floor(ppos.x)) / static_cast<int>(ts);
                int py = static_cast<int>(std::floor(ppos.y)) / static_cast<int>(ts);
                bool planted = false;
                for (int oy=-1; oy<=1 && !planted; ++oy) {
                    for (int ox=-1; ox<=1 && !planted; ++ox) {
                        int tx = px + ox;
                        int ty = py + oy;
                        if (tx < 0 || ty < 0 || static_cast<unsigned>(tx) >= map.width() || static_cast<unsigned>(ty) >= map.height()) continue;
                        sf::Vector2f center((float)tx * (float)ts + ts * 0.5f, (float)ty * (float)ts + ts * 0.5f);
                        float dx = center.x - ppos.x;
                        float dy = center.y - ppos.y;
                        if (dx*dx + dy*dy > interactDistSq) continue;
                        if (map.isTilePlantable((unsigned)tx, (unsigned)ty)) {
                            // consume one seed and spawn crop
                            if (player->inventory().removeItemById(foundSeedId, 1)) {
                                // seed id has form seed_<cropid>
                                std::string cropId = foundSeedId.substr(std::string("seed_").size());
                                entities.push_back(std::make_unique<Crop>(game.resources(), center, cropId, 3, 6.f));
                                planted = true;
                                std::cerr << "Planted seed " << foundSeedId << " at tile " << tx << "," << ty << "\n";
                                // mark tile as non-plantable (set to Empty) to avoid replanting same spot
                                map.setTile((unsigned)tx, (unsigned)ty, TileMap::Empty);
                            }
                        }
                    }
                }
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

    // draw dialog UI on top
    dialog.draw(win);

    // draw inventory UI last
    if (inventoryUI) inventoryUI->draw(win);
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
    std::cerr << "Loaded game\n";
}