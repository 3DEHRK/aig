Rails & Logistics — Feature Design and Technical Plan

Overview

Rails are tile-based transport networks for moving goods, water drums, and people. This plan covers track placement, cart behavior, routing, and automation primitives.

Components
- Track tile: placed on TileMap, connects to cardinal neighbors if tracks exist.
- Switch/junction tile: enables routing decisions; stores a default direction and optional programmable priority.
- Cart: entity with capacity, speed, and current route plan.
- Depot/Transfer node: storage with insertion/extraction rules.

Placement rules
- Tracks snap to tile grid; adjacent track tiles connect automatically.
- Collision: tracks cannot be placed on unbuildable tiles (water deep, rock).

Routing
- Graph-based routing using Dijkstra/A* on the rail graph; support for priority nodes.
- Switch control API for players to set default directions or automated logic.

Automation
- Schedulers and triggers to dispatch carts (time-based or conditional).
- Inserters to move items between inventories and carts.

UI
- Rail tool: ghost preview sprite, snapping, visual connect lines, and lane direction arrows.

Testing
- Unit tests for track connectivity and pathfinding.

Todo
- Tram physics and curved tracks support.
