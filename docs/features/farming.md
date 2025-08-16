Farming — Feature Design and Technical Plan

Overview

Detailed implementation plan for seeds, soils, crop lifecycle, and farming interactions.

Data models
- SeedDefinition JSON schema (id, name, stages, baseGrowthTime, waterCurve, lightPref, soilAffinity, yield, mutationChance).
- TileSoil state: fertility, pH, organic, moisture, lastTilledTime, appliedAmendments.
- CropEntity: currentStage, growthTimer, seedId, plantedAtTick, health, diseaseStatus, visualState.

Growth algorithm
- Each sim tick evaluate growth rate modifier based on tile moisture (WaterSim), temperature (PowerSim/HVAC), light level, and soil fertility.
- Growth increment: dt * baseRate * modifier. When growthTimer >= stageThreshold, advance stage.

Interactions
- Tools: till (change tile.type and modify fertility), water (temporarily water tile or use sprinklers), fertilizer application (increase fertility, risk of burn if overapplied).
- Harvest: when crop at maxStage, interact to harvest, produce items based on yield table and quality modifiers.

Edge cases
- Adjacent plants may suffer or benefit (allelopathy), pests may spread if nearby infected tiles.

Integration
- Harvested items feed into Inventory; crafting recipes use harvested goods.
- Visual overlays for soil moisture and fertility.

Testing
- Deterministic seed tests: simulate fixed environment and assert final yield/quality.

Todo
- Crossbreeding mechanics and mutation experiments.
