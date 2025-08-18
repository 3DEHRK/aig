# SFML Game Framework

## Overview
A C++17 / SFML (3.x) top‑down pixel game prototype framework inspired by Stardew‑like simulation RPGs. Provides structured subsystems: state management, entities, resources, input, basic farming & combat, respawn & navigation aids, minimap with fog‑of‑war, persistence (JSON), and configurable UI overlays.

## Directory Structure (Key)
- `src/core/` Game loop (`Game`), abstract `State` base.
- `src/states/` Concrete states: `PlayState`, `HiddenRealmState` (prototype realm).
- `src/entities/` Player, NPC, HostileNPC, Crop, Rail, ItemEntity, Projectile, Altar, HiddenLocation, AnimatedSprite helpers.
- `src/world/` `TileMap` (terrain tiles + soil moisture/fertility + exploration fog arrays).
- `src/systems/` Dialog, Inventory, Quest (stub), SaveGame (helpers), SoundManager.
- `src/ui/` Inventory UI overlay.
- `src/tools/` Rail build tool.
- `assets/` Fonts & textures.
- `docs/` Design & feature specs + roadmap (`features/todo.md`).

## Build
```
mkdir -p build && cd build
cmake ..
cmake --build . -j
./bin/sfml-game-framework
```
Requires SFML 3 found via CMake (`find_package(SFML 3 ...)`).

## Runtime Controls (Default)
Inventory I
Moisture overlay M, Fertility overlay N, Fertilize current tile F
Interact / Plant / Confirm E
Shoot projectile (dir = movement) Space (cooldown)
Rail build tool toggle B (click to place/remove rails)
Respawn marker toggle P (world + minimap)
Respawn distance text toggle O; Units toggle tiles/pixels T
Minimap toggle U; Scale cycle J (tile pixels 2/3/4)
Minimap view rectangle toggle V; Minimap entity icons toggle G
Death penalty on/off Y
Save K  Load L
Help overlay H (lists these controls)

## Implemented Features
- Player movement with collision against solid tiles (axis separated AABB).
- Soil system: per-tile moisture & fertility arrays (float 0..1) with simple watering & fertilizing actions; overlays (M/N) + per-frame soil update & decay.
- Crop lifecycle: plant seeds (inventory id prefix `seed_`), growth timer, harvest removal, tile restored to Plantable.
- Inventory (stacking add/remove, JSON serialize/deserialize, UI panel).
- Combat: projectile spawning (direction based on last movement input), hostile NPC taking damage, death removal, floating combat text.
- Hostile NPC: simple chase AI, health & death.
- Respawn system: altar can set respawn point; death screen with countdown; configurable visibility & distance arrow (off‑screen) with unit formatting.
- Death penalty toggle: optional 10% stack reduction (excluding seeds) on respawn.
- Minimap: full map tile rendering with fog‑of‑war (exploration radius), adjustable tile pixel size (2–4), optional camera view rectangle overlay, entity icons (category colored), player marker, respawn marker.
- Help overlay: on-demand key reference & toggles.
- Persistence (JSON): player position, health, death state, respawn point, inventory, crops, map (tiles, soil, explored fog), hostile NPCs, and UI/toggle preferences (respawn marker/distance/units, minimap settings, death penalty, help overlay).
- Rail tool: interactive placement/removal translating map rail tiles → rail entities (sync function).
- Hidden realm prototype state (trigger via altar item requirement) placeholder.

## Persistence Schema (Excerpt)
```
player: { x, y, health, respawn_x, respawn_y, dead,
  show_respawn_marker, show_respawn_distance, respawn_distance_tiles,
  show_minimap, death_penalty, minimap_tile_px, minimap_view_rect,
  minimap_entities, help_overlay, inventory:[ {id,count} ... ] }
map: { w,h,ts, tiles[], soilMoisture[], soilFertility[], explored[] }
crops: [ {id, x, y, stage, ... } ]
hostiles: [ { x,y, health } ]
```
Backward compatibility: Optional keys are tested with `contains` before use.

## Testing & Extensibility Hooks
- Deterministic save/load (K/L) allows test scenario setup & regression snapshots.
- Inventory & crop JSON: quick injection / scenario scripting.
- Minimap scaling & toggles expose rendering logic for visual tests.
- Clear seams for future test harness: expose `PlayState` update & headless mode (TODO: add compile flag to skip window creation for CI).

### Planned Test Harness Improvements
- Headless mode macro `BUILD_HEADLESS` to stub SFML window & render calls.
- Assertion utilities (bounds, non-negative health, tile invariants) compiled in debug.
- Input replay log (recorded sequence -> deterministic playback for regression).

## Roadmap Snapshot (See `docs/features/todo.md` for full list)
Near-term:
1. Complete save coverage (altar state, rail graph meta, dialog state)
2. Soil → crop growth influence (link moisture/fertility to timers)
3. Spatial partition for projectile vs hostiles broad-phase
4. Particle & SFX feedback pass (harvest, hit, altar)
5. Data-driven config (JSON) for tunables
6. Multiple respawn points & selection UI
7. Hidden realm persistence (no full state reset on return)

## Coding & Design Conventions
- C++17, smart pointers for ownership, single responsibility per file.
- Avoid magic numbers: elevate tunables to config (in progress).
- JSON serialization guarded with existence checks for forward schema evolution.

## Making It Testable
Current steps taken:
- Pure logic separated from rendering where feasible (TileMap soil update, inventory logic, serialization routines) enabling unit-style testing.
- Deterministic per-frame input edge tracking; easy to feed synthetic key events (extend `InputManager`).
Planned additions (not yet implemented):
- Extract simulation tick functions (e.g., `updateSoil`, `updateCombat`) into standalone modules callable by tests.
- Provide `Game::runOneFrame(dt)` for fixed-step harness.
- Introduce compile-time flag to bypass actual rendering for automated CI.

## Next Steps
Focus on deepening the farming loop, broadening persistence, and introducing debugging/test harness utilities before expanding large new systems (power/water).

## License
MIT

---
Generated & maintained iteratively; see commit history & `summary.md` for full architectural narrative.