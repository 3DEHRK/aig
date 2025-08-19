# Transcendence & Ritual Systems

## Concept
A layered metaphysical progression where the player channels biome energy, stabilizes realm fractures, and performs altar rites to unlock macro modifiers (Ascension Traits) that alter future cycles.

## Resource Types
- Resonance (ambient accumulation from healthy biome tiles)
- Corruption (negative buildup from instability events)
- Catalyst Items (dongles, purified shards, boss echoes)
- Runes (crafted inscription pieces defining ritual logic)

## Altar Tiers
1. Crude Altar: Basic activation; opens small portal window (timed realm visit).
2. Harmonized Altar: Supports rune slots (2) influencing portal effects.
3. Ascendant Matrix: Full ritual grid (hex layout) enabling multi-effect chaining.

## Rune Types
- Amplify (boost resource gain)
- Purify (convert corruption → resonance)
- Anchor (extends realm visit duration)
- Transmute (change item classes during ritual)
- Echo (stores % of surplus power into next cycle)

## Ritual Flow
1. Configure runes (if tier allows) + slot catalysts.
2. Verify stability threshold (resonance ≥ cost, corruption ≤ limit).
3. Channel Phase (interactive mini-loop: maintain harmonic meter by timed inputs or resource routing).
4. Resolution Phase: Apply rune graph evaluation producing Ascension Trait(s).
5. Portal Open: Hidden Realm accessible; traits become pending until safe return.

## Hidden Realm Design
Procedurally seeded pockets with:
- Fractured Resource Nodes (high yield, unstable)
- Echo Shrines (lore + minor permanent buff choices)
- Wraith Hazards scaling with corruption index
Timer persists; leaving early reduces risk but lowers trait potency scaling multiplier.

## Ascension Traits Examples
- Soil Memory: baseline fertility after reset raised by +15%.
- Harmonic Flow: irrigation pulses cost 20% less power.
- Echoed Harvest: first harvest each day yields +1 produce.
- Corrupt Dampening: passive corruption gain -25%.

## Instability & Backfire
If channel phase fails (meter collapse): Backfire Event:
- Local contamination spike radius.
- Hostile surge spawn.
- Potential rune fracture (lose one rune item).

## Persistence Across Cycles
On Ascend (player triggers full cycle reset):
- Preserve: Ascension Traits, codex entries, cosmetic unlocks.
- Reset: Crops, infrastructure, routine inventories.
- Partial: Power tech tier maybe gated behind trait synergy.

## Data & Save
Store: altar tier, rune inventory, configured runes (graph edges), resonance/corruption pools, trait list.

## UI
Ritual Configuration Panel (drag runes into sockets), harmonic meter (waveform alignment mini-game), portal timer HUD, trait selection modal on resolution.

## Balancing
- Early ritual costs modest; failure penalties mild.
- Higher-tier rituals require multi-system readiness (stable power + low contamination + stored catalysts).

## Extensibility
Add rune by specifying input symbol, effect function, synergy hooks (pairs → composite outcome), and icon asset reference.
