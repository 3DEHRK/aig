# Water Simulation & Irrigation

## Purpose
Model moisture distribution, purity, and flow to support farming depth, contamination management, and power integration (pumps).

## Layers
1. Surface Moisture (soil tiles)
2. Network Pipes / Channels
3. Storage Tanks / Reservoirs
4. Purification Units (filters)

## Data Model
```
struct WaterNode { Vec2i tile; NodeType type; float volume; float capacity; float purity; };
struct Pipe { NodeId a,b; float throughput; float loss; };
```

Soil moisture is separate but receives pulses from emitters.

## Tick Algorithm (fixed step)
1. Equalize volumes across connected components within throughput constraints.
2. Apply losses & evaporation (temp dependent).
3. Emit moisture pulses to soil (IrrigationEmitter nodes) lowering their volume.
4. Purifiers consume power → raise purity of passing volume.
5. Contamination diffusion from high contamination soil patches back into network baseline (if leaky pipes).

## Moisture Application
Each emitter has radius R; tiles within R receive `delta = basePulse * (1 - distance/R)` clamped. Overwatering increases runoff event chance spawning puddle tiles (slows actors briefly).

## Purity Effects
- High purity (>0.85) grants small growth speed buff.
- Low purity (<0.4) increases contamination accumulation on soil.

## Integration with PowerSim
- Pumps require continuous power proportional to vertical lift simulated abstractly as distance tier.
- Emergency shutdown on power fail to prevent backflow contamination spike.

## Visualization
- Overlay gradient for moisture; animated droplets rising from emitters.
- Pipe flow arrows brightness scales with throughput.

## Events
- OnPurityChange(node, old, new)
- OnEmitterPulse(node, affectedTiles)
- OnContaminationBackflow(componentId)

## Save Schema
Persist nodes (type, volume, capacity, purity) and pipe connections; soil moisture stored per tile.

## Extensibility
Additional node: Thermal Condenser (extracts moisture from air under certain weather), Rune Infuser (late—adds resonance to water improving growth).
