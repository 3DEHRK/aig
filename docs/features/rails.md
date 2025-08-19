# Rails & Logistics Network

## Purpose
Reduce hauling friction, introduce spatial optimization puzzles, and later integrate with power & sensing layers.

## Core Components
- Track Segment (straight / corner / junction)
- Cart Entity (capacity, speed, scriptable route)
- Loader / Unloader (interfaces inventory <-> cart)
- Buffer Chest (intermediate storage)
- Signal Post (controls cart flow / priority)

## Data Model
```
struct TrackTile { bool occupied; JunctionType jt; uint8_t links; };
struct Cart {
  Vec2f pos; Vec2f dir; float speed; float baseSpeed; float loadFactor; Route route; Inventory inv; };
```

Route: sequence of waypoints (tile coords) + loop flag.

## Movement Logic
- Carts advance along param t in current segment; reaching end chooses next link based on route index or signal directive.
- Speed = baseSpeed * f(loadFactor) * g(powerBoost).

## Loading Algorithm
At loader tile tick:
1. Determine available item stacks in source inventory.
2. Transfer until cart slot full or loader throughput cap reached.
3. Mark throughput usage for balancing.

## Signals
Simple state machine: RED (halt), GREEN (go), YIELD (enter if no cart in conflicting junction). Later integrate detection sensors computing occupancy.

## Automation Depth
Tier 1: Manual route carts (player sets loop).
Tier 2: Conditional routing via item filters at junction loader nodes.
Tier 3: Power-assisted maglev segments (higher speed, higher power draw).
Tier 4: Quantum relay pads teleport specific item classes (late meta, high ritual upkeep).

## UI / UX
- Placement preview highlights connection candidates.
- Route Editor: select cart → click sequence of anchors → confirm to apply.
- Throughput overlay shows segment utilization heat (color gradient).

## Event Hooks
- OnCartArrive(node)
- OnItemTransferred(cart, itemId, qty)
- OnSignalChange(signalId, newState)

## Balancing Targets
- Base cart speed: 120 u/s; capacity 12 stacks.
- Loader throughput: 4 items / sec baseline.
- Power maglev multiplier: 2.0x speed, + constant power drain.

## Persistence
Serialize track grid (bit-pack links), carts (route index, cargo), signal states.

## Extensibility
Add new cart module (cooling, speed amp) by augmenting speed & capacity formula plug-ins.
