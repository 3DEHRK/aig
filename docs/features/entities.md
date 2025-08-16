Entities & AI — Feature Design and Technical Plan

Overview

Detailed plans for NPCs, Hostile NPCs, and general entity lifecycle.

NPCs
- Data model: id, name, schedule (time→position/action), preferences, inventory, relationshipState.
- Behavior tree: high-level actions (wander, goTo, work, interact) with conditions.
- Pathfinding: A* grid search on walkable tiles; dynamic re-plan if blocked.

Hostile NPCs
- Simple state machine: idle → chase → attack → retreat (for health low).
- Attack effect: damage reduces player health or removes items from inventory.

Performance
- LOD: for distant NPCs use simplified behavior updates (less frequent ticks).

Integration
- Events: NPCs can trigger narrative events when specific systems are in place.

Testing
- Unit tests for pathfinding and schedule execution.
