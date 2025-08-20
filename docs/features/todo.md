# TODO – Farming Game Build Checklist

## Systems
- [x] ✅ Implement **HarvestFX system**:
  - Squash (0–90 ms), pop + particles (90–180 ms), fade+shrink (180–260 ms), loot magnetize (260–450 ms).
  - Spawn "+N" popup and flash outline.
  - Play pluck + rustle + coin SFX.
  - Apply 3 px screen shake.
  - add needed sounds at location as txt placeholder eg coin_pickup.ogg

- [x] ✅ Implement **Hold-to-Harvest**:
  - Key press starts harvesting at cursor tile.
  - If key held, sweep 3×3 rect.
  - Continue harvesting as cursor moves.
  - Queue actions; next starts at 120 ms overlap.

- [x] ✅ Implement **Magnet Pickup**:
  - Loot within 2.5 tiles accelerates toward player.
  - Reach player ≤300 ms.

- [x] ✅ Implement **Plant-System**:
  - Plant with cursor

- [x] ✅ Implement **Water/Fertilize All**:
  - Apply to all tiles in radius N (start 5).
  - Skip mature plants.

- [x] ✅ Implement **Global Buff System**:
  - Timed modifiers (e.g., moisture decay -30% for 120 s).
  - Show buff icon in HUD.

## Progression
- [x] ✅ Implement **Contracts Board** received from NPC:
  - Load from `contracts.json`. (add sample data)
  - Each contract = input items + reward items.
  - On completion, add reward to inventory.

- [x] ✅ Implement **Trader**:
  - Vending machine interface.
  - Reads static trade offers from `trades.json`. (add sample data)
  - Simple exchange, no dialogue.

- [x] ✅ Implement **Unlock Rules**:
  - On first harvest of plant → unlock seed blueprint.
  - On first rail placed into biome → unlock biome-specific seeds.

## Immersion
- [x] ✅ Implement **Day/Night Cycle**:
  - Gradient lighting.
  - Implement Lamps that emit halo if powered.

- [x] ✅ Implement **Micro-Wind Crop Shader**:
  - Sway amplitude tied to weather/biome.

- [x] ✅ Implement **SFX/Ambience**:
  - Footstep sounds with surface differentiation (soil / default / rail).
  - Random ambient one-shots (wind, bird, leaves) on jittered timer.
  - Placeholder sound asset filenames added under `assets/sfx/` (README lists required .ogg files).

- [x] ✅ Implement **Diegetic Tile Indicators**:
  - Local stake overlays show moisture bar & fertility pip (toggle with O).
  - Color gradients dry→wet and poor→rich; limited radius to reduce clutter.

- [x] ✅ Implement **World Decals**:
  - Wheel ruts align with sample rail track.
  - Oil stain near altar area.
  - Scattered leaf debris in forest patch.