#include "HostileNPC.h"
#include "Player.h"
#include <cmath>
#include "../world/TileMap.h"
#include "Entity.h" // for resolveAxis

void HostileNPC::update(sf::Time dt) {
    if (!playerTarget) return;
    float ds = dt.asSeconds();
    if (rageDuration == 0.f) {
        if (auto *tj = g_getTunablesJson()) {
            if ((*tj).contains("hostile") && (*tj)["hostile"].contains("grunt")) {
                auto &gj = (*tj)["hostile"]["grunt"];
                if (gj.contains("rage_speed_mult")) rageSpeedMult = gj["rage_speed_mult"].get<float>();
                if (gj.contains("rage_duration")) rageDuration = gj["rage_duration"].get<float>();
            }
        }
    }
    if (rageTimer > 0.f) { rageTimer -= ds; if (rageTimer < 0.f) rageTimer = 0.f; }
    float currentSpeed = speed * (rageTimer > 0.f ? rageSpeedMult : 1.f);

    if (flashTimer > 0.f) {
        flashTimer -= ds;
        sf::Color base = sf::Color::Red;
        if (flashTimer > 0.f) {
            float f = flashTimer / 0.15f; // 0..1
            // interpolate toward white
            base.r = (uint8_t)(base.r + (255 - base.r) * f);
            base.g = (uint8_t)(base.g + (255 - base.g) * f);
            base.b = (uint8_t)(base.b + (255 - base.b) * f);
        }
        shape.setFillColor(base);
    } else if (shape.getFillColor() != sf::Color::Red) {
        shape.setFillColor(sf::Color::Red);
    }

    auto ppos = playerTarget->position();
    auto b = getBounds();
    sf::Vector2f center(b.position.x + b.size.x*0.5f, b.position.y + b.size.y*0.5f);
    float dx = ppos.x - center.x; float dy = ppos.y - center.y; float dist = std::sqrt(dx*dx + dy*dy);
    if (dist > attackRange) {
        if (dist > 0.f) {
            sf::Vector2f dir(dx/dist, dy/dist);
            shape.move(dir * currentSpeed * ds);
        }
    } else {
        attackTimer -= ds;
        if (attackTimer <= 0.f) {
            attackTimer = attackCooldown;
            std::cerr << "HostileNPC attacks player!\n";
            playerTarget->takeDamage(contactDamage);
            // apply a brief reactive nudge to player (visual feedback)
            {
                sf::Vector2f ppos2 = playerTarget->position();
                sf::Vector2f center2(b.position.x + b.size.x*0.5f, b.position.y + b.size.y*0.5f);
                sf::Vector2f dirNorm = { ppos2.x - center2.x, ppos2.y - center2.y };
                float ln = std::sqrt(dirNorm.x*dirNorm.x + dirNorm.y*dirNorm.y);
                if (ln > 0.f) dirNorm = {dirNorm.x/ln, dirNorm.y/ln}; else dirNorm = {1.f,0.f};
                // simple offset (non-colliding, player movement solver will clamp next frame if inside wall)
                playerTarget->applyMove(dirNorm * 6.f);
            }
        }
    }
}

void HostileNPC::takeDamage(float amount) {
    health -= amount;
    std::cerr << "HostileNPC took " << amount << " damage. health=" << health << "/" << maxHealth << "\n";
    onDamaged(amount);
    if (health <= 0.f) {
        std::cerr << "HostileNPC died.\n";
        // basic drop table stub: emit items into world (requires ItemEntity / Item)
        extern nlohmann::json* g_getTunablesJson();
        // we need access to game resources & entity list; for now, signal via stdout; actual spawning handled in PlayState scan
    }
}

void HostileNPC::nudge(const sf::Vector2f& delta) {
    if (!tileMap) { shape.move(delta); return; }
    sf::Vector2f move = delta;
    auto tryAxis = [&](float& mv, bool horizontal){
        if (mv == 0.f) return;
        sf::FloatRect b = getBounds();
        // future position after movement on that axis
        if (horizontal) b.position.x += mv; else b.position.y += mv;
        unsigned ts = tileMap->tileSize();
        int tx0 = std::max(0, (int)std::floor(b.position.x / ts));
        int ty0 = std::max(0, (int)std::floor(b.position.y / ts));
        int tx1 = std::min((int)tileMap->width()-1, (int)std::floor((b.position.x + b.size.x - 0.0001f) / ts));
        int ty1 = std::min((int)tileMap->height()-1, (int)std::floor((b.position.y + b.size.y - 0.0001f) / ts));
        // check collisions; if any solid found, clamp movement to 0 (small impulses acceptable to discard)
        for (int ty = ty0; ty <= ty1; ++ty) {
            for (int tx = tx0; tx <= tx1; ++tx) {
                if (tileMap->isTileSolid(tx,ty)) { mv = 0.f; return; }
            }
        }
    };
    // separate axis resolution (simple since knockback is small)
    float mx = move.x; float my = move.y;
    tryAxis(mx, true);
    shape.move({mx,0.f});
    tryAxis(my, false);
    shape.move({0.f,my});
}
