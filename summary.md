# 🐮🦄🌀 HORYN SUMMARY BANNER 🌀🦄🐮

# Project Summary (Comprehensive)

## 1. Vision & Current Trajectory
A modular C++17 / SFML 3 foundation for a systemic top‑down simulation RPG blending: farming, logistics, light combat, environmental simulation (planned water & power), discovery / lore, and ritual transcendence loops. Current codebase represents an early vertical slice prototype: core loop (move → interact → plant → fight) exists, while automation, deep simulation, persistence breadth, and polishing layers are still embryonic.

## 2. Implemented Runtime Systems (State: Prototype)
- Game Loop & State Management: PlayState and HiddenRealmState (temporary portal scene) using polymorphic State base (not included in scan but implied). Transition triggered by Altar activation.
- Rendering: Immediate mode draws via each entity’s draw method; no batching; basic view follow for player with clamp to map extents.
- Input: Central InputManager (not re-documented here) polled per frame; supports wasKeyPressed style edge detection.
- Resource Loading: ResourceManager caches textures & fonts (simple map + lazy load). No eviction or hot reload.
- Entities: Monolithic base class with pure virtual update/draw/getBounds/interact. Implementations:
  * Player: Movement, projectile spawning, inventory, interaction driver.
  * NPC (passive) & HostileNPC (simple chase & attack AI, health/damage logic minimal).
  * Crop: Basic growth staging (growth logic limited; lacks soil/moisture data coupling yet).
  * ItemEntity: World pickup wrapper around Item objects.
  * Rail: Placeholder track entity (logistics network not yet simulated).
  * Projectile: Straight-line motion + simple collision resolution vs HostileNPC.
  * HiddenLocation: Discovery mechanic granting item (content presently hollow after revert).
  * Altar: Ritual activation gating realm transition (consumes required item IDs).
  * ExampleSpriteEntity / AnimatedSprite / Animation component: Early animation scaffolding.
- Inventory & Items: Stack-based inventory with add/remove by id; Items minimal (id, name, description, quantity, texture path). No durability, rarity, or metadata.
- Dialog: Simple queue of strings advanced via input; overlays game but halts world updates (except special cases for realm confirmation).
- SaveGame (skeleton): Limited to player position & inventory IDs; tilemap JSON serialization stubbed; many systems unsaved.
- SoundManager: SFML 3 compatible buffer cache + ephemeral sound instances; not integrated into gameplay events beyond potential manual calls.

## 3. World & Spatial Systems
- TileMap: Provides tile-based world with plantability checks and collision queries (AABB). Discovery flags for hidden locations. No chunk streaming, no layering (foreground/background), no pathfinding grid metadata.
- Collision: Manual AABB overlap loops (O(n^2) worst-case on projectiles vs hostiles). No spatial partition (quad tree / uniform grid) yet.
- Camera: Single view lock on player; no smoothing or camera zones.

## 4. Current Content Layer
- Items: Seeds, sample produce placeholders; dongle item used for altar activation conceptually (id consumed).
- Entities: One basic hostile NPC archetype; no differentiation in stats or behaviors.
- Ritual / Realm: Altar activation triggers short HiddenRealmState (placeholder) with timed auto-return (re-instantiates PlayState — losing prior state continuity).

## 5. Removed / Reverted Documentation Impact
Design documents for farming depth, power, water, transcendence, rails, quests, UI, and hostile ecology were previously drafted and are now empty. This represents a knowledge base loss and increases risk of implementation divergence. The summary should therefore also serve as an interim spec anchor until docs are restored.

## 6. Architectural Gaps & Technical Debt
| Area | Current State | Risk | Recommended Near-Term Action |
|------|---------------|------|------------------------------|
| State Preservation | Realm transition rebuilds PlayState | Loss of world continuity | Snapshot & restore pattern (serialize entity+map deltas) |
| Entity Scaling | Single inheritance interface | Hard to layer cross-cutting features (health, power port) | Introduce lightweight components/mixins (HasHealth etc.) |
| Collision Performance | Linear scans | Performance degradation with entity growth | Implement uniform grid partition for broad-phase |
| Save System | Minimal scope | Player progress loss / inconsistent tests | Central SaveRegistry mapping system IDs → JSON segments |
| Config & Balancing | Hard-coded constants | Recompile for tuning | External JSON/CSV for tunables loaded at startup |
| Resource Paths | Inline string literals | Fragile / duplication | Central asset manifest + ID-to-path mapping |
| Input | Fixed mappings | Accessibility limitation | Add remap layer persisted to config file |
| Error Handling | Silent catches (swallow exceptions) | Debug difficulty | Structured logging with severity + fallback visuals |
| Rendering | Individual draw calls | Fill rate / CPU overhead later | Sprite batching (atlas + instancing), layered render queues |
| Sound Integration | Bare utility not used | Low feedback clarity | Event-driven audio dispatcher & mixing groups |
| Testing | TESTME doc only | Regressions unnoticed | Add automated runtime assertions / lightweight test harness |

## 7. Simulation Systems (Planned but Missing)
- Farming Ecology: Soil fertility, moisture, contamination, rotation effects — current Crop lacks environment linkage.
- Water Network: Moisture propagation, emitter nodes, purity metrics.
- Power Grid: Generation, storage, consumers with priority & load balancing.
- Logistics (Rails): Pathing, carts, loaders, throughput modeling.
- Quests / Directives: Adaptive goals & codex persistence.
- Ritual & Ascension: Rune graph evaluation, realm instability metrics, trait persistence across cycles.

## 8. Data & Persistence Strategy Proposal
Design a modular save architecture:
```
root {
  version: 1,
  world: { seed, time, tiles: [...], discoveries: [...] },
  entities: [ {type:"Player", data:{pos:[x,y], hp, inventory:[...] } }, ... ],
  systems: {
    farming: {...},
    power: {...},
    water: {...},
    quests: {...},
    ritual: {...}
  },
  meta: { transcendenceTraits: [...], cycle: n }
}
```
Version gating allows migrations; each system registers serializer/deserializer lambdas.

## 9. Performance Considerations & Scaling Plan
| Feature | Current Complexity | Target Optimization |
|---------|--------------------|---------------------|
| Entity Update Loop | O(n) sequential | Bucket by frequency; parallel non-interacting groups later |
| Projectile Collision | O(p*h) | Spatial grid cells referencing entity indices |
| Resource Loading | On-demand synchronous | Preload critical set + background streaming for large textures |
| Save/Load | Full synchronous write | Chunked async write for large worlds (post-MVP) |

Memory Footprint: Keep each tile ≤ 16 bytes for large maps (pack fertility/moisture into uint16). Use SoA (structure of arrays) for simulation-heavy layers later.

## 10. Testing & Debug Tooling Roadmap
- Debug Overlay: FPS, entity counts, update timings.
- Inspect Mode: Click entity → dump JSON state to console.
- Deterministic Seed Replay: Record input events for regression runs.
- Assertion Layer: e.g., validate no entity leaves world bounds post-update.

## 11. Risk Analysis (Forward-Looking)
| Risk | Description | Mitigation |
|------|-------------|------------|
| Feature Creep | Expanding systems before core feel is polished | Lock milestone scope (MVP vertical slice) |
| Data Loss / Corruption | Evolving save schema breaks old loads | Versioned schema + migration routines + test saves |
| Performance Regression | Adding systems without profiling | Integrate lightweight profiler markers early |
| Tech Debt (God Objects) | PlayState bloats with every subsystem | Extract managers (FarmingSystem, CombatSystem) now |
| Player Feedback Insufficiency | Actions feel dull w/o SFX/VFX | Prioritize feedback pass before deep new systems |
| Complexity of Ritual Layer | Over-engineered meta loop early | Implement bare “portal activation + timed reward” first |
| Lack of Tools | Difficult balancing without analytics | Add logging hooks (growth times, power deficits) |

## 12. Proposed Iteration Sequence (Incremental Realism)
1. Stabilize: Fix realm state preservation, fill empty design docs.
2. Feedback: Integrate SFX & placeholder particles for core actions.
3. Farming Depth: Add soil tile data & moisture tool → growth influenced by environment.
4. Persistence: Serialize crops + discovery + altar active state.
5. Logistics Foundation: Rail graph & basic cart movement (no loaders).
6. Power Skeleton: Node registry + single generator powering a dummy consumer (visual overlay).
7. Water Skeleton: Emitters increasing moisture; unify tick scheduler.
8. Hostile Expansion: Another archetype + scaling spawn logic.
9. Ritual Tier 1: Simple resonance cost + portal timer reward.
10. Quests/Directives: Minimal directive suggestions (inventory & farming based).

## 13. Refactoring Blueprint
Introduce directories:
```
src/sim/farming
src/sim/power
src/sim/water
src/ecs (future components)
```
Add central `GameContext` struct referencing managers (Resources, Input, Audio, Systems). Decompose PlayState responsibilities:
- InteractionSystem
- CombatSystem
- FarmingSystem
- RitualSystem
Each system exposes update(dt) and event hooks.

## 14. Data-Driven Config Prototype
Create `data/config.json`:
```
{
  "player": {"speed": 200, "maxHealth": 100},
  "combat": {"projectileSpeed": 300},
  "farming": {"baseGrowthRate": 0.05},
  "audio": {"masterVolume": 0.8}
}
```
Load once; expose via Config singleton or context.

## 15. Audio & Visual Feedback Plan
Events → Audio IDs → layered sound with pitch variance. Particle stubs (struct Particle {pos, vel, life, spriteId}) managed by a ParticleSystem; simple pool allocation. High-impact events (altar activation) combine screen flash, radial particles, low-frequency rumble.

## 16. Hidden Realm Evolution Path
Phase A: Timed isolated arena (resource nodes + risk). Phase B: Procedural pocket generation seeded by altar rune config. Phase C: Realm modifiers feed back into base world (e.g., growth speed buff) on successful extraction.

## 17. Coding Standards & Conventions (Establish)
- Naming: PascalCase for types, camelCase for methods/vars, UPPER_SNAKE for constants.
- Avoid raw new/delete; continue using smart pointers.
- Logging macro with level (LOG_INFO, LOG_WARN, LOG_ERR) and subsystem tag.
- Single Responsibility: Each file ≤ ~300 lines where possible; split large states.

## 18. Security / Robustness Considerations
Even offline game benefits from input sanitization for data files. Validate JSON keys & ranges. Avoid undefined behavior (check dynamic_cast results, bounds). Add clang-tidy static analysis configuration in future.

## 19. Tooling & CI Prospects
- Add compile commands (CMake) for clangd / static analysis.
- Optionally integrate a lightweight Github Actions workflow (build + run headless tests).
- Script asset audit (list missing texture files vs manifest).

## 20. Summary & Strategic Focus
Core scaffolding is sound for a prototype. Immediate risk is divergence due to absent detailed specs and overloading PlayState. Short-term investment in documentation, modular systems extraction, and save breadth will reduce future rewrite cost. Prioritize player feedback loops (audio/particles) to ensure iterative feel testing informs deeper system design rather than building large silent simulations.

---
Prepared as an authoritative interim design & architecture reference until detailed feature documents are reinstated.
