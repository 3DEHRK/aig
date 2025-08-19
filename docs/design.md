# Game Design Overview

## Vision
A cozy-but-deep top‑down 2D simulation RPG blending farming, exploration, automation, arcane technology, and transcendental progression. The player arrives in a fading biome and rekindles it through cycles of cultivation, discovery, and ritual ascension. Early loop is tangible (soil, water, crops). Midgame adds logistics (rails, power, water networks) and ecology management. Lategame introduces metaphysical layers (hidden realms, altar rites) that recontextualize base systems rather than discard them.

## Core Pillars
1. Layered Progression: Each new system amplifies—not replaces—previous layers.
2. Meaningful Automation: Tools (rails, power, irrigation) reduce tedium while creating new optimization puzzles.
3. World Reactivity: Soil health, water purity, and biome stability shift based on player actions.
4. Discoverable Lore Through Systems: Hidden locations, relic dongles, and procedural inscriptions hint at cosmic structure.
5. Sensory Feedback & Micro Delight: Tight input feel, satisfying harvest/effects cadence, audio layering.
6. Player Authorship: Flexible layout & network design encourage unique base identities.

## Gameplay Loop Tiers
Tier 1 (Day 0–2): Explore, plant basic crops, gather dongle fragments, simple NPC interactions.
Tier 2: Expand plots, unlock irrigation prototypes, craft rails, encounter hostile fauna.
Tier 3: Power grid & processing (dryers, arc kilns, seed analyzers). Emergent resource ratios appear.
Tier 4: Hidden realm excursions for rare catalysts; altar activations gating transcendence metrics.
Tier 5: Meta-loop: Ascend (soft reset selective layers), introduce new global modifiers (seasonal anomalies, waveform weather) boosting variety.

## Progression Axes
- Agricultural Depth (crop variety → hybridization → mutagenic cultivation).
- Infrastructure (manual carry → carts → powered conveyors → quantum relays).
- Knowledge (dongles → inscriptions → compiled codices → transcendence runes).
- Ecology (soil fertility → biome resonance → realm stability).
- Personal Capability (tools, artifacts, ritual boons).

## Economy & Balancing Framework
- Define Base Value (BV) per raw item; derived goods use transformation multipliers and time weighting.
- Maintain Efficiency Frontier: Automation should shift from linear labor saving to logistic constraint solving (layout, throughput, energy budget).
- Introduce Soft Caps via diminishing soil fertility, power load inefficiency, and water salinity accumulation.

## System Interlock Map
Farming ↔ WaterSim (moisture & irrigation) ↔ PowerSim (pumps) ↔ Rails (distribution) ↔ Quests (demand signals) ↔ Hostiles (pressure & area denial) ↔ Transcendence (meta modifiers altering all coefficients).

## Technical Architecture Snapshot
- Entity Component Lean Hybrid: Single polymorphic base for now; structured evolution path to EC separation if needed.
- Deterministic Subsystems: Power and water ticks in fixed-step micro-sim (e.g. 200ms) decoupled from render frame.
- Persistence Layers: World (terrain, networks), Entities (state bundles), Meta (transcendence seeds), Analytics (play metrics optional).

## Content Scaling Strategy
Start Minimal Vertical Slice: 2 crops, 1 hostile, 1 rail cart, 1 generator, 1 altar ritual. Expand breadth once feel loop is tight.

## Player Motivation Curves
Short-term: Crop maturation, dialog snippet reveals, micro-upgrades.
Mid-term: Automating a production chain & unlocking hidden map sectors.
Long-term: Achieving stable realm resonance enabling a clean transcendence cycle.

## Risk & Pressure Sources
- Temporal Weather Shifts temporarily alter growth coefficients.
- Hostile migrations target high-density infrastructure.
- Entropy Accumulation: Over-farming pushes soil into fatigue states requiring rotation.

## Sensory Direction
- Minimal palette with saturation spikes on key feedback (harvest glow, portal flare).
- Layered diegetic audio loops (wind, water drip, biome pulse) + procedural chimes on rare events.

## Extensibility Guidelines
- Each system doc defines: Data Schema, Tick Model, Event Hooks, Save Fields.
- Avoid hard-coded magic numbers; centralize tunables in JSON / data tables.

## Success Criteria (MVP)
- 30 minute session produces emergent planning decisions.
- Player triggered at least one automation upgrade and one lore discovery.
- Clear next-goal affordances without explicit quest scripting.
