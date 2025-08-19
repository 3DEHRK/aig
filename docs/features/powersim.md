# Power Simulation

## Purpose
Provide energy as a unifying constraint linking automation, advanced tools, and ritual apparatus. Encourages network planning (generation, storage, transmission, load prioritization).

## Core Objects
- Generator: Produces watts based on fuel curve or environmental input.
- Storage Cell: Capacitor with charge / leak.
- Conduit: Transfers power (with distance loss & capacity cap).
- Consumer: Declares load profile (continuous, burst, standby).
- Relay / Switch: Conditional routing based on signals (later signal layer).

## Data Model
```
struct PowerNodeId { uint32_t idx; };
struct PowerNodeBase { Vec2i tile; NodeType type; };
struct Generator : PowerNodeBase { float output; float fuel; FuelMode fuelMode; };
struct Storage   : PowerNodeBase { float capacity; float charge; float leakRate; };
struct Conduit   : PowerNodeBase { float maxFlow; }; // edges implicit via adjacency / links
struct Consumer  : PowerNodeBase { float demand; float priority; float buffer; bool powered; };
```
Adjacency Graph stored as vector<SmallVec<NodeId>> links.

## Tick Model
Fixed timestep (e.g. 0.2s). Steps:
1. Collect generator available output (consider fuel depletion + efficiency drop near overheating).
2. Distribute to storages (respect capacity) then to consumers by priority queue.
3. Compute deficits; unpowered consumers emit Event(PowerFail).
4. Update storage leakage & dynamic efficiency modifiers.

## Loss & Efficiency
Transmission loss proportional to cumulative conduit length and load ratio. Encourage use of localized micro-grids and later high-grade relays (reduced loss).

## Load Prioritization
Consumers sorted by (priority desc, deficit ratio desc). Allows life-support or irrigation pumps to receive power first.

## Burst Consumers
Have buffer and activation threshold (e.g. Auto-Harvester needs 40 energy burst). Buffer fills when surplus available; triggers action when full.

## Control Signals (Future)
Add simple boolean channels (S0..S7) toggled by sensors (SoilSensor high stress → enable IrrigationPump). Relays open/close circuits based on channel state.

## Ritual Interaction
Ritual constructs temporarily spike grid demand / export stability pulses reducing realm instability (if adequately powered). Underpowered ritual backfires (corruption release).

## Visualization
- Overlay heatmap: node voltage / flow saturation.
- Pulse animation traveling conduits proportional to flow.

## Balancing Targets (Early)
- Basic Generator: 10 u/s base, 60 s per fuel unit, 80% efficiency.
- Loss: 1% per tile beyond 5 tiles chain baseline.
- Storage leak: 0.1% charge per second.

## Save Schema
Persist nodes array with type tag, state floats, link indices. Version for migrations.

## Extensibility
New generator type adds formula: output = f(environmentalInput, upgradeLevel). Consumers can declare dynamic demand curve callback.
