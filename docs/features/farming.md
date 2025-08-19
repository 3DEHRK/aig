# Farming System

## Purpose
Provide an expandable scaffold where early manual planting evolves into a managed agro-ecological network integrating irrigation, soil chemistry, and eventually metaphysical growth modifiers.

## Core Concepts
- Tile Soil State: fertility, moisture, tilled flag, crop root stress, contamination level.
- Crop Archetype: growth stages, base duration, water curve modifier, fertility demand profile, byproduct chance table.
- Hybridization: adjacency-based cross-pollination producing seed variants with stat mutations.
- Seasons / Microclimate: later phase—dynamic modifiers via biome resonance & realm shifts.

## Data Model (per tile)
```
struct SoilTile {
  bool tilled;
  float fertility;      // 0..1
  float moisture;       // 0..1
  float contamination;  // 0..1 (salinity / toxins)
  float biomeResonance; // 0..1 (late meta buff)
  CropInstance* crop;   // nullable
}
```

## CropInstance
```
struct CropInstance {
  std::string id;
  float growth;        // 0..1 normalized
  float stress;        // 0..1 (slows growth, increases mutation)
  uint8_t stage;       // discrete stage index
  uint8_t maxStages;
  bool ready;          // harvestable
  MutationFlags mutation; // hybrid trait bits
}
```

## Growth Algorithm
1. Compute effectiveRate = baseRate * f(moisture) * g(fertility) * h(temp) * realmModifier * (1 - stressPenalty).
2. Increment growth by dt * effectiveRate.
3. If stage threshold crossed: stage++ with feedback event (particles + SFX).
4. If moisture < wiltingThreshold or contamination high → increase stress.
5. stress > mutationTrigger adds random trait (color shift, yield variance, time variance).

### Moisture Curve f(moisture)
Piecewise: under-water (0.5), optimal band (1.0), over-water (0.7). Supports irrigation tuning.

### Fertility Function g(fertility)
`g = 0.4 + 0.6 * fertility` with diminishing returns past 0.85 (soft cap smoothing).

## Player Actions
- Hoe (till) → create tilled tile (resets moisture to baseline).
- Plant seed (consumes item, spawns CropInstance).
- Water (increases moisture; may splash adjacent via tool upgrades).
- Fertilize (increase fertility using compost / arc residue).
- Harvest (yields produce + chance secondary outputs).
- Scan (midgame analyzer reveals hidden mutation stats).

## Hybridization Mechanics (Midgame)
Each midnight tick:
- For each crop, gather adjacent (Von Neumann + diagonals) distinct species IDs.
- If ≥2 species and crop is at penultimate stage: roll cross event.
- Generate hybrid seed with inherited traits:
  - GrowthRate = weighted avg ± variance based on stress.
  - YieldModifier = max(parentYield) ± small mutation.
  - ResilienceTrait flags if parents share resilience.

## Soil Fertility Cycle
- Harvest consumes fertility proportional to crop demand tier.
- Leaving field fallow regenerates fertility slowly.
- Crop rotation bonus: sequence diversity reduces future fertility drain.
- Over-fertilization raises contamination → requires leaching via irrigation flush.

## Contamination Sources
- Overuse of synthetic booster items.
- Power machinery leakage (if placed atop soil without insulation).
- Realm backlash events (failed ritual nearby).

## Feedback & UX
- Subtle soil color shift for fertility bands.
- Moisture sparkle overlay after watering fading over time.
- Growth stage chime (pitch ascends with stage index).
- Stress shimmer effect (wavy distortion) warns before yield penalty.

## Balancing Targets (Early)
- Base wheat: 4 minutes total growth (4 stages) with ideal care.
- Water decay: moisture drifts toward 0.3 baseline at 0.05 / sec.
- Fertility drain: common crops 0.02 per harvest; rare 0.05.

## Automation Interfaces
- Irrigation Nodes: connect to water network, emit moisture pulses (AOE radius).
- Soil Sensors: push tile telemetry into power network bus (for UI dashboards).
- Auto-Harvester (late): requires stable realm resonance to unlock; consumes power + coolant.

## Events / Hooks
- OnPlant(tile, cropId)
- OnStageAdvance(cropRef, oldStage, newStage)
- OnHarvest(cropRef, yieldItems)
- OnMutation(cropRef, mutationMask)
- OnSoilDegrade(tile, delta)

## Save Fields
Serialize per tile fertility, moisture, contamination, crop (id, growth, stage, mutation bits), and global mutation registry.

## Extensibility
Add new crop by data file specifying: stages, textures, baseRate, fertilityDemand, moistureOptRange, yieldTable, mutationWeights.

# Comprehensive Farming System Design

## Overview
The comprehensive farming system is an intricate and dynamic network of agro-ecological interactions, player-driven management, and automated systems. It evolves from simple manual planting to a complex, self-sustaining ecosystem with minimal player intervention.

## Phases of Farming System

### 1. Manual Farming (Early Game)
- Players manually till soil, plant seeds, water, and fertilize crops.
- Basic crops with simple growth requirements and linear progression.
- Limited by player knowledge, resources, and tool capabilities.

### 2. Advanced Farming (Mid Game)
- Introduction of hybrid crops with cross-pollination and genetic mutations.
- Players manage more complex growth cycles and resource management.
- Introduction of seasons and microclimates affecting crop growth.
- Basic automation with irrigation nodes and soil sensors.

### 3. Automated Farming (Late Game)
- Fully automated farming systems with minimal player intervention.
- Complex agro-ecological networks that are self-sustaining.
- Advanced crop genetics with tailored growth traits and resilience.
- Integration of metaphysical elements for enhanced growth and automation.

## Detailed System Components

### Soil Management
- **Tilling**: Prepares the soil for planting, affecting moisture and fertility.
- **Moisture Control**: Irrigation systems that adjust moisture levels based on crop needs and environmental conditions.
- **Fertility Management**: Use of organic and inorganic fertilizers to manage soil fertility, with a risk of contamination if mismanaged.

### Crop Management
- **Planting**: Selection of crop archetypes with specific growth requirements and byproducts.
- **Growth Monitoring**: Observing crop growth stages, stress levels, and readiness for harvest.
- **Hybridization**: Cross-pollination of crops to create hybrids with desirable traits.

### Resource Management
- **Water Management**: Efficient use of water resources through irrigation and rainwater harvesting.
- **Nutrient Management**: Balancing soil nutrients to optimize crop growth and prevent contamination.
- **Energy Management**: Using renewable energy sources to power automated systems.

### Pest and Disease Management
- **Monitoring**: Regularly check crops for signs of pests and diseases.
- **Control**: Use of organic pesticides, beneficial insects, and other eco-friendly methods to control pests and diseases.

### Harvesting and Processing
- **Harvesting**: Collecting mature crops, with consideration for optimal timing to maximize yield and quality.
- **Processing**: Preparing harvested crops for sale, consumption, or further processing (e.g., milling wheat into flour).

### Marketing and Sales
- **Market Research**: Understanding market demand, pricing, and trends.
- **Sales Channels**: Direct sales to consumers, farmers' markets, or wholesale to retailers.
- **Branding and Promotion**: Creating a brand for farm products and promoting it through various channels.

## Endgame Goals
- Achieve a fully automated, self-sustaining farming system.
- Discover and cultivate all crop varieties and hybrids.
- Master the balance of the agro-ecological network for optimal production.
- Explore and integrate metaphysical elements for enhanced farming capabilities.

## Potential Future Expansions
- Introduction of new crop types, animals, and farming technologies.
- Expansion of the farming area to new biomes and realms.
- Advanced genetic engineering for crops and animals.
- Integration of trade, diplomacy, and competition with other farming entities.

