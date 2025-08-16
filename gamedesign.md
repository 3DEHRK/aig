# Game Design Document — "Aurelia: The Otherworld Farm"

Version: 0.9
Last updated: 2025-08-16
Author: design spec (for implementation team)

---

High-level elevator pitch

Aurelia is a 2D top‑down, pixel‑art, otherworldly farming‑and-exploration game. It blends intimate character stories, slow-burn romance, and layered systems design. Players restore a strange farm at the edge of reality, solving engineering puzzles (water and power networks), mastering crop life cycles, constructing rail and logistics systems, and uncovering a fragmented ecology and interpersonal lore. Every technical decision matters: water flow, pump scheduling, power routing, soil chemistry and seed selection influence crop outcomes and narrative opportunities.

Design goals

- Deep emergent systems: layered mechanical systems (hydraulics, power, climate control, rail logistics) that interact and produce emergent gameplay.
- Emotional narrative through systems: character bonds grow through shared tasks, quests, and the consequences of technical choices.
- Exploration & discovery: a mystery‑rich world with unique biomes, rare seeds, artifacts, and cryptic lore fragments.
- Player agency & configuration: many decisions are persistent and reversible; systems are highly configurable (pump speeds, pipe layouts, filter types).
- Accessible core loop: planting, tending, harvesting, building — with tools that scale from handheld to automated.

Target platform & tech constraints

- Single‑player desktop (Windows / macOS / Linux) using C++17 + SFML 3.x.
- Deterministic sandbox simulation where possible (to permit replay/bug reproduction).
- Data files use human‑readable JSON for saves and prototypes.

Core pillars (what makes the game feel unique)

1. Systems as storytelling: mechanical choices reveal and affect story beats.
2. Engineering puzzles at farm scale: water routing, timed pumps, pressure balancing, and seasonal power management.
3. Ritualized farming: crops are more than resources — many are story keys, alchemical reagents, or spirits.
4. Discovery-driven geography: hidden biomes, ruins, and curio items shaped by environmental puzzles.

---

Table of contents

1. World & Narrative
2. Characters & Relationships
3. Core Gameplay Systems
4. Engineering Systems (water, power, climate)
5. Farming & Agriculture Depth
6. Construction, Logistics & Rails
7. AI & Entities
8. UI/UX and Controls
9. Progression & Quests
10. Assets & Art Direction
11. Technical Architecture & Data
12. Roadmap & Milestones (from README)
13. Testing, Metrics, and Deliverables

---

1) World & Narrative

Setting
- A twilight pastoral world folded into seams of reality. The farm sits atop an old, half‑buried machine; the ground hums and sometimes sings. Flora and fauna are uncanny: glowing lichens, floating seeds, and rivers that loop back through space.
- Tone: melancholy + wonder. The world suggests previous civilizations and a slow apocalypse, but also the possibility of repair.

Lore mechanics
- Fragments: small texts, ghost‑voices, and found artefacts reveal bits of the world. Completing mechanical milestones unlocks new fragments.
- The farm is the player’s chosen place of refuge: repairing systems gradually reactivates a local memory engine revealing full story arcs.

Exploration incentives
- Rare seeds, water sources, and ancient machinery are scattered across a map with layered biomes.
- Some locations require mechanical solutions to access (flood control, power routing, rail bridges).

2) Characters & Relationships

Character design approach
- Core cast: 6–8 fully written characters with strong motivations. Each NPC has schedules, emotional states, and preference profiles.
- Relationship simulation: bonds are influenced by actions (gifts, quests, shared engineering successes/failures). Relationship depth increases with time, meaningful choices and system mastery.

Examples
- Mara — an archivist who loves fragile blossoms; she values long‑term crops and controlled climates.
- Ond — a rail engineer who respects elegant systems; he gives mechanical upgrades when trusts rise.

Relationship systems
- Intimacy metric per character that increases via events and gifts (items, solved puzzles). At key thresholds, character events unlock.
- Events are tied to system achievements: e.g., building a functioning irrigation loop for Mara triggers an emotional cutscene.

3) Core Gameplay Systems

A) Player: movement, tools, stamina
- Movement: WASD/arrow keys. Tools: hand‑trowel, pipe wrench, pump controller, rail placer.
- Stamina: optional; using heavy tools consumes energy requiring food/rest.

B) Inventory & items
- Slot‑based inventory with stack sizes and item metadata.
- Item types: seeds, harvested crops, engineered parts (pipes, pumps), artifacts, fuel items, and crafted items.
- Items contain metadata: id, name, description, recipes, rarity, seed properties (growth time, water needs, thermal preference).

C) Crafting & Recipes
- Workbench crafting: recipes consume items and produce parts (filters, pipes, rails, power nodes).
- Some recipes are discoverable via experimentation (combine reagents to create new seed variants).

D) Save/Load
- JSON saves including world tilemap, systems state (pumps, pipes), NPC states, and player inventory.
- Version field for forward compatibility.

4) Engineering Systems (in depth)

Overview
- Engineering is tile‑based: pipes, pumps, reservoirs, valves, filters, power lines, turbines and generators.
- All systems operate with simplified physics models tuned for gameplay.

A) Water system
- Units: volume (liters), flow rate (L/s), pressure (abstracted).
- Components: source (spring), reservoir, pipe, valve, pump (configurable speed), sprinkler (zone emitter), filter, hydro‑turbine.
- Flow simulation: discrete time steps per frame or per simulation tick (configurable rate). Use graph traversal from sources through pumps to sinks.
- Valves & branching: valves can throttle (0–100%). Pumps consume power proportional to flow rate.
- Evaporation & leakage: porous tiles and weather affect efficiency.
- Soil water buffer: each soil tile stores moisture value. Crop water consumption drains this buffer. Watering events can increase moisture.

B) Power system
- Units: Watts (W), Energy (J) as integrated over time.
- Sources: generator (biomass burned), solar panels (time/day dependency), hydro turbines (driven by water flow), magical cores.
- Distribution: nodes and lines. Lines have resistance (loss). Nodes have storage (batteries) with charge/discharge limits.
- Consumers: pumps, climate units, lamps, processing machines.
- Failure modes: overloads trip breakers; balancing requires scheduling and storage.

C) Climate & Grow rooms
- Controlled rooms with temperature and humidity that change growth rates.
- HVAC units consume power and use water for humidification.

Design considerations
- Make systems deterministic and tunable: provide constants for flow vs. power costs, leak rates, and crop water needs.
- Use event hooks so narrative triggers listen to system events (e.g., "network reached full pressure" → unlock a memory fragment).

5) Farming & Agriculture Depth

Seed model
- Each seed has: growthStages, baseGrowthTime, waterRequirement curve, lightPreference (shade/neutral/sun), soilAffinity (acid/alkaline), yieldVariants, rarity, mutationChance.
- Variants: combining seeds or environmental stress can create variants (crossbreeding, mutations).

Soil & tiles
- Tile properties: type (soil, rock, water), fertility, pH, organic matter, moisture.
- Tilling & amendments: player can till, add compost, lime, or fertilizer which modifies soil properties and affects yield.

Crop lifecycle
- Planting consumes a seed item and spawns a Crop entity with growth timer and stage textures.
- Growth rate influenced by local tile conditions: moisture, temperature, light, adjacent plants (allelopathy).
- Pests & diseases: chance to appear; treatable with crafted remedies.

Harvesting & processing
- Crops yield raw items; some require processing (drying, fermenting, kiln) to become marketable or usable in recipes.
- Quality system: yields have quality stats based on care; quality influences value and character reactions.

6) Construction, Logistics & Rails

Construction philosophy
- Tools escalate from hand placement to blueprint automation. Early game is hands‑on; late game supports macro placement and automation.

Rails & logistics
- Track tiles connect into networks. Modes: hand cart, automated tram, goods transfer.
- Carts: configurable speed, capacity, and maintenance. Use tracks to transport water drums, seeds, harvested goods.
- Switches and junctions: programmable routing by logic gates or simple 'priority' rules.

Storage & automation
- Storage nodes with filters (only accept certain item ids), and inserters to move items between storage and machines.
- Schedulers: time-based or condition-based triggers to run pumps, move carts, or open valves.

7) AI & Entities

NPC behavior
- Schedules: day/night cycles; NPCs perform tasks, visit locations, and react to world state.
- Emotional states: mood variables, influenced by player actions and system state.
- Trade & services: NPCs can buy/sell, repair equipment, or craft items.

Hostile agents
- Define simple hostile NPCs with pathfinding, attack ranges, and loot drops. Damage can remove items or affect soil/structures.

Pathfinding
- Use A* on walkable map; dynamic obstacles (temporary farm objects) integrated via cost updates.

8) UI/UX and Controls

Principles
- Present systems clearly: overlays for water flow, power lines, soil moisture and rail grid.
- Progressive complexity: early UI shows minimal info; advanced toggles expose debugging overlays and simulators.

Key UI elements
- HUD: time of day, energy, water storage, quick toolbelt.
- Overlays: toggle water flow visualization, power heatmap, soil moisture map, rail connectivity.
- Build mode: ghost preview, snapping to tile grid, rotation, and quantized placement.

Controls
- Movement: W/A/S/D (or arrow keys)
- Interact: E
- Build/Toolwheel: mouse + number keys
- Inventory: I
- Toggle overlays: F1 (water), F2 (power), F3 (soil), B (rail tool)

9) Progression & Quests

Progression model
- Soft progression: unlock tools, improved blueprints, and expanded irrigation/power options.
- Skill systems: optional—skills improve efficiency (less power usage, faster pumps).

Quest types
- Personal (character drives): help NPCs, relationship events.
- Systemic (engineering): fix the ancient pump, restore the hydro line.
- Exploration: find ruins, gather rare seeds, decode fragments.

10) Assets & Art Direction

Visuals
- Pixel art, limited palette per biome. Use atmospheric lighting and particle effects for odd flora.
- Sprites: tile size 32x32 (configurable). Character sprites 32×48 frames with simple animation.

Audio
- Ambient soundscapes, subtle field recordings, mechanical hums for powered systems, and emotional leitmotifs for characters.

11) Technical Architecture & Data

Code organization (suggested)
- core/: Game loop, main, State management.
- entities/: Entity classes (Player, NPC, Crop, Rails, Machines, Projectiles).
- systems/: Inventory, Quest, SaveGame, WaterSim, PowerSim.
- resources/: ResourceManager, Asset pipeline.
- tools/: Build tools (RailTool, PipeTool), Debug overlays.

Simulation design
- Decouple render from simulation: run water/power ticks at fixed timestep (e.g., 10Hz) while rendering at variable fps.
- Use an event bus for cross‑system triggers (e.g., "PumpStarted", "TileFlooded", "FragmentRevealed").

Data formats
- JSON for saves and prototypes (seed definitions, item registry, recipes).
- TileMap format: width/height/tileSize + per‑tile type and metadata.

Configuration & tuning
- Central config JSON for global constants (flow coefficients, power costs, growth multipliers) to tune balance.

12) Roadmap & Milestones (from README)

Current ordered list (preserved):
1. ✅ Robuste Inventory-UI — basic grid UI implemented (drag & drop, icon support); needs visual polish and keyboard shortcuts
2. ✅ Anpflanzen von Crops mit Samen — basic crop growth & harvest implemented
3. ✅ Eisenbahnsystem mit baubaren Schienen — basic rail build tool implemented (press 'B' to toggle build mode; click to place/remove rails on tiles). Persistence in save/load pending.
4. ⬜ Feindliche NPCs, welche attackieren — partial implemented (HostileNPC prototype that chases and 'attacks')
5. ⬜ Abwehrwaffe gegen NPCs — pending
6. work on bug list below
7. add ingame message / log system for player infos and logging
8. reitterate over all features improving usability, quality, add detail
9. add water, farmland and different seeds... expand farming mechanism in creative ways

Suggested next milestones (detailed)
- M1 (core polish): Inventory persistence, save/load of items and map, UI polish (tooltips). 2–4 weeks
- M2 (systems): Implement WaterSim and PowerSim basic systems, soil moisture propagation. 4–8 weeks
- M3 (logistics): Advanced rails & carts with automation and storage nodes. 3–6 weeks
- M4 (narrative): Full character arcs, events and memory engine. 4–8 weeks

13) Testing, Metrics, and Deliverables

Testing
- Unit tests for Inventory, TileMap, serialization, and sim tick determinism.
- Integration tests: reproduce seeded runs (save, load, simulate N ticks, assert outcomes).
- Manual tests: TESTME.md checklist (keep updated).

Metrics
- Track user sessions during playtests: time to first irrigation loop, average crops harvested per day, average number of mechanical failures.

Deliverables per milestone
- Code + JSON config, smoke tests, sample maps, art placeholder sets, and a playable build.

---

Appendix A — Seed/Item schema (example)

{
  "id": "seed_wheat",
  "name": "Wheat Seed",
  "type": "seed",
  "growthStages": 3,
  "baseGrowthTime": 600.0, // seconds
  "waterCurve": [0.2, 0.8], // min optimal range (normalized)
  "lightPref": "neutral",
  "soilAffinity": {"fertility": 0.6},
  "yield": [{"id":"crop_wheat","count":1,"qualityFactor":1.0}]
}

Appendix B — Tuning constants (suggested keys in config JSON)
- sim.tickHz: 10
- water.unit: liters
- pump.basePowerPerLps: 0.5
- hydro.turbine.efficiency: 0.4
- soil.evaporationPerTick: 0.001

---

Notes for the engineering team
- Start with minimal, testable simulation models. Keep code modular and expose config constants.
- Prioritize save/load determinism and clear debugging overlays early — they speed iteration.
- Keep narrative assets separate from mechanics; event triggers should call into the narrative subsystem rather than hardcode story.

If you want, I can scaffold: JSON seed registry, WaterSim and PowerSim prototypes, and a set of unit tests to validate deterministic ticks. Tell me which system to implement next and I will create a concrete task plan and the initial code stubs.
