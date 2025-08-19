# TODO / Implementation Roadmap (Restructured)

## Open Issues (Reported)
- [x] NPC dialog off-screen when moving; should anchor to screen-space HUD panel, not world position. ✅ (now rendered in default view)
- [x] Inventory (I) key pressed but inventory UI not visibly opening (regression) – FIX: expanded tracked keys list.
- [x] Interact/Plant (E) no longer plants seeds when no entity targeted – FIX: restored planting branch logic.
- [x] Inventory items invisible – FIX: added item label text (name snippet + stack) in each slot.

## Onboarding / Demo Scenarios (Planned)
- [x] Farming demo: pre-seeded mini plot (guides planting, watering, optional fertilizing, harvest) ✅
- [x] Rail demo: prebuilt short loop with a moving cart the player can board (after cart system implemented) + instructional overlay. ✅
- [ ] Integrate a brief in-game note referencing behavior rules / autonomous progression toggle (optional dev build aid).

## Rail Demo Test Checklist
- [ ] Player spawns near a prebuilt rail loop with a cart moving along it.
- [ ] Cart can be boarded (optional: press E near cart).
- [ ] Cart follows waypoints and loops.
- [ ] Loader/unloader tiles can be assigned and function.
- [ ] Items can be loaded into cart and unloaded at destination.
- [ ] Cart route can be edited (add/remove waypoints, toggle loop).
- [ ] Save/load preserves cart state, waypoints, and logistics assignments.
- [ ] Demo overlay or dialog explains controls (Z: route mode, 1/2: loader/unloader, B: rail tool).

## Completed
- ✅ Minimap (fog-of-war, scale J, view rect V, entity icons G)
- ✅ Respawn navigation aids (marker P, distance O, unit toggle T, arrow)
- ✅ Death penalty toggle (Y) with inventory stack reduction (exempt seeds)
- ✅ Help / controls overlay (H)
- ✅ Dialog state persistence (active & remaining lines)
- ✅ Input remapping JSON loader scaffold (bindings.json optional)
- ✅ Starter content pack bootstrap (data/items_basic.json + registry)
- ✅ Tunables JSON scaffold (data/tunables.json loaded at startup)
- ✅ Headless executable scaffold (sfml-game-framework-headless placeholder)
- ✅ Close game with ESC / window X
- ✅ Binding persistence (bindings.saved.json auto load/save)
- ✅ Damage hook + applyDamage funnel (initial unification groundwork)
- ✅ Regen curve & soil tunables (player.regen_curve_exponent, soil targets/decay)
- ✅ Projectile speed & hostile contact damage tunables
- ✅ Basic hit flash feedback (player + hostile)
- ✅ Projectile knockback + lifetime tunable (basic displacement)
- ✅ Hostile rage (temporary speed boost after hit)
- ✅ Basic crop codex text snippets (JSON-backed overlay toggle C)
- ✅ Track tile orientation metadata & serialization (orientation bits, overlay, helpers, persistence)
- ✅ Hostile variant persistence (grunt/tank type saved & restored)

---
## Phase 0: Core Stability & Persistence Baseline
Goal: Solid playable loop (move, collide, interact, plant, fight, save/load) with crash-free foundations.
- [x] Refine entity architecture (collision helpers + damage hook + knockback safety + rage) ✅
- [x] Complete save/load coverage (rail meta, overlays, progression flags) ✅
- [x] Input remapping layer (loader + persistence; in-game UI deferred) ✅
- [x] Apply tunables to runtime parameters (player, hostile stats incl rage, farming growth, projectile speed/lifetime/knockback, regen curve, soil, contact damage) ✅

## Phase 1: Farming Core (Baseline Mechanics Only)
Goal: Make farming feel meaningful before adding depth.
- [x] Soil → crop growth influence (simple moisture & fertility multipliers) ✅
- [x] Watering tool item + tuned moisture decay (basic tool effect & splash) ✅
- [x] Basic fertilizer item (single tier) & UI feedback (prototype consumption + fertility boost) ✅
- [x] Harvest yield variability (fertility based) ✅
- [x] Various plants with different needs (growth time via tunables mapping) ✅
- [x] Simple crop sprites/placeholder frames per stage for each plant (size + color scaling) ✅
- [x] Early progression content: unlock fertilizer after harvesting cumulative 10 crops (temp trigger) ✅
- [x] Basic crop codex text snippets (C key overlay) ✅

## Phase 2: Basic Combat & Hostiles Foundation
Goal: Lightweight pressure; postpone advanced ecology systems.
- [x] Simple hostile spawn timer & spawn zones ✅
- [x] Basic hit feedback polish (flash, small knockback) ✅
- [x] Drop table stub (hostile -> item entities) ✅
- [x] Rudimentary threat scaling placeholder (time + movement → spawn accel & cap) ✅
- [x] Content: 2 hostile variants (melee grunt, slow tank) ✅ (spawn weight scales via threat; variant type not yet persisted in saves)
- [x] Loot variety: common fiber, rare crystal (future ritual use) ✅ (standardized item IDs fiber / crystal_raw)
- [x] Basic consumable: small healing salve craftable from fiber (test crafting flow later) ✅ (3 fiber -> 1 salve_small; use heals 25)

## Phase 3: Rails & Logistics (Intro Tier Only)
Goal: Movement of items in carts on simple tracks; advanced routing later.
- [x] Track tile orientation metadata & serialization ✅
- [x] Cart entity prototype (moves along waypoints on rails)
- [x] Manual route assignment UI (list of waypoints) ✅
- [x] Loader / unloader minimum viable transfer (single item per tick) ✅
- [x] Cart waypoint + loader/unloader persistence (save/load cart.waypoints, loop, logistics tiles) ✅
- [x] Content: Starter logistics quest ("Automate a wheat haul") awarding extra cart speed module placeholder ✅

## Phase 4: Quests & Directive Scaffold
Goal: Motivational layer for early gameplay.
- [~] Directive evaluator engine (initial scaffold: Directive struct, evaluation & HUD display) ⚠️
- [~] Quest data structure + serialization (basic Quest already implemented earlier; may expand) ⚠️
- [~] Minimal journal UI (panel toggle + listings prototype) ⚠️
- [x] Directive persistence & load fallback ✅
- [x] Sample starter quest & directive templates (chain logic) ✅
- [x] Content: Initial quest chain (3 steps: Plant seeds -> Harvest 5 crops -> Build first rail segment) ✅

(Items marked [~] are partially implemented and need completion / polish before full ✅)

## Phase 5: Ritual & Hidden Realm (Baseline)
Goal: Give altar progression purpose; delay complex instability.
- [ ] Altar upgrade tiers (statically defined JSON)
- [ ] Hidden realm basic instance save/restore
- [ ] Portal visual placeholder (color pulse)
- [ ] Content: Tier 1 upgrade recipe (uses crystal + herb) unlocking realm entry

## Phase 6: Farming & Logistics Depth (Advanced)
Goal: Add nuance after core loops proven fun.
- [ ] Growth stress & mutation hooks
- [ ] Multi-tier fertilizers & diminishing returns
- [ ] Advanced rail junction logic & signaling
- [ ] Loader throughput balancing & UI overlay
- [ ] Content: New crop archetype (night-bloom) influenced by day/night (placeholder cycle)

## Phase 7: Power & Systems Simulation
(Deferred until movement & farming loops solid.)
- [ ] Power node registry & tick loop
- [ ] Generator + storage + consumer minimal trio
- [ ] Power overlay (voltage / flow coloring)
- [ ] Content: Prototype generator + lamp building assets (placeholder art)

## Phase 8: Ecology Pressure & Advanced Hostiles
- [ ] Threat level computation (farming intensity, pollution proxy)
- [ ] Hostile spawn scheduler waves
- [ ] Additional hostile archetypes + simple behaviors
- [ ] Damage types groundwork (resistances framework)
- [ ] Content: Elite hostile event (rare spawn) dropping ritual catalyst

## Phase 9: Ritual Depth & Realm Dynamics
- [ ] Portal particle system
- [ ] Realm instability index & escalating wraith spawns
- [ ] Realm resource anomalies (boosted fertility zones)
- [ ] Content: Realm resource nodes (give boosted fertilizer ingredients)

## Phase 10: Polishing & Player Feedback
- [ ] Particle system module (harvest, hit, portal)
- [ ] Layered audio mixing (ambient + events)
- [ ] Screen shake / camera easing utilities
- [ ] HUD expansions (power, moisture trend graphs)
- [ ] Improved minimap overlays (gradient fog, zoom levels)
- [ ] Day/Night lighting & ambient color shift
- [ ] Ambient flora placement pass (trees, shrubs, ground cover) + simple persistence
- [ ] Passive fauna/critters (butterflies, fireflies) low-cost sprite wanderers
- [ ] Weather prototype (rain increases soil moisture, light fog)
- [ ] Biome definition stub (tag tiles/regions -> spawn tables for flora/fauna)
- [ ] Ambient soundscapes (wind, insects) tied to time of day/biome
- [ ] Content polish pass: distinct tiles for fertile vs exhausted soil, unique altar upgrade sprite variants

## Phase 11: Optimization & Structural Refactors
- [ ] Spatial partition (uniform grid) for broad-phase queries
- [ ] Batch rendering (atlas / instancing path)
- [ ] Data-driven tunables refinement & live reload

## Phase 12: Late-Game / Stretch Systems
- [ ] Seasonal waveform system
- [ ] Advanced rail automation (junction filters, logic)
- [ ] Quantum logistics tier
- [ ] Ascension meta modifiers cycle
- [ ] Taming / conversion of constructs
---
## Tracking Notes
- Move any newly finished task to Completed with a ✅.
- Avoid starting later phases until all prior phase essentials checked (except low-risk polish).
- Keep TESTME.md updated when persistence keys or input bindings change.
