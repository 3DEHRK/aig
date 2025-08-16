PowerSim — Feature Design and Technical Plan

Overview

PowerSim models generation, storage, and consumption of electrical (or "magical") energy. It interacts with WaterSim (hydro turbines produce power) and buildings (HVAC, pumps). Focus is on gameplay clarity: simple flow, losses, and scheduling.

Core components
- Generator: biomass burner, solar panel, magical core.
- Line: connects nodes; has resistance and capacity.
- Battery: stores energy; charge/discharge limits.
- Consumer: pumps, heaters, sprinklers, lights, machines.
- Breaker: trips on overload; must be reset.

Simulation
- Time-discrete ticks aligned with sim.tickHz or an independent powerTickHz.
- For each tick: compute total generation, apply to demands based on priority and storage state, apply line losses, track battery charge.

Integration
- UI overlays: power flow, node charge, alerts.
- Save/Load serialization: node states, battery charge, generator schedules.

Tuning
- Constants: generator base output, line resistance per tile, battery capacities.

Testing
- Unit tests for generator/battery interplay, load-shedding logic.

Todo
- Implement grid balancing algorithms and scheduling policies.
