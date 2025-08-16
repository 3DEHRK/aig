WaterSim — Feature Design and Technical Plan

Overview

WaterSim is a tile-based hydraulic simulation used to route, store, and deliver water to soil tiles, machines, and storage. It emphasizes gameplay clarity and performance over physical accuracy. The simulation runs at a fixed tick rate (configurable via sim.tickHz) and updates volumes, flows, and pressures on discrete components.

Goals
- Provide a playable hydraulic model for irrigation puzzles.
- Be deterministic, debuggable, and fully serializable in saves.
- Fast enough to run on large maps (optimize for sparse networks).

Core concepts and components

- Units & time
  - Volume unit: liters (config.water.unit)
  - Tick duration: 1.0 / sim.tickHz seconds

- Components (tile entities)
  - Source: infinite or finite water source (spring, well)
  - Pipe: connects neighboring nodes; has diameter and base flow resistance
  - Pump: actively moves water; configurable speed (L/s) and direction
  - Valve: adjustable open fraction (0.0..1.0)
  - Reservoir: stores water (capacity L) and provides gravity head
  - Sprinkler/Emitter: consumes water and deposits moisture to adjacent soil tiles
  - Filter: optionally removes contaminants and slows flow
  - HydroTurbine: consumes flow to produce power (connects to PowerSim)

Data structures
- Node graph: sparse graph of nodes for connected pipes, pumps, etc. Each node stores current volume, capacity, and connections to neighbors with edge resistances.
- Tile registry: mapping from tile coordinates to component instances.

Simulation algorithm (per tick)
1. Build/update active graph only for components with non-zero connections (sparse update).
2. For each source, inject volume into its node.
3. Solve flow distribution using iterative relaxation (Gauss-Seidel style) or simplified max-flow per edge constrained by pump capacities and valve fractions.
4. Move water along edges: adjust node volumes, track flow direction and magnitude per-edge for visualization.
5. Apply sink consumption: sprinklers drain volume and increase soil moisture for adjacent tiles.
6. Evaporation/leakage: global leakage rates reduce volumes slowly.

Performance considerations
- Use delta ticks and limit graph updates to changed areas.
- Represent edges as structs with precomputed inverse resistances.
- Avoid global solvers; use localized iterative methods limited to small subgraphs.

Integration points
- TileMap: add a tile object to store water component metadata and underlay sprite/overlay for pipes.
- UI overlays: water flow heatmap, per-edge arrows displaying L/s.
- Save/Load: serialize every node's volume, pump states, valve settings, and pipe layout.

Scripting & tuning
- Config JSON keys: sim.tickHz, pipe.baseResistance, evaporation.rate, pump.basePowerPerLps.
- Expose debug commands: step once, visualize flows, export graph to JSON.

Testing
- Unit tests: small graphs with known inputs and expected outputs (1->2->sink).
- Integration tests: seed/soil irrigation end-to-end (plant at position X receives N liters after T ticks).

TODOs
- Implement Packet-based flow for very large networks if needed.
- GUI widgets for pump control and valve sliders.
