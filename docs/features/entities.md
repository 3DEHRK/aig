# Entity Taxonomy & Lifecycle

## Categories
1. Player & Avatars
2. NPC (social / quest / hostile)
3. Resource Nodes (rock, tree, crystal growths)
4. Crops & Flora (dynamic growth logic)
5. Infrastructure (rails, junctions, power pylons, irrigation emitters)
6. Items-in-World (dropped stacks, relic dongles)
7. Projectiles & Effects (transient)
8. Ritual Constructs (altar, rune pylons, portal anchors)
9. Hidden / Discovery Artifacts (invisible triggers, lore tablets)

## Base Interface (Current)
```
class Entity {
  virtual void update(Time dt);
  virtual void draw(RenderWindow&);
  virtual FloatRect getBounds() const; // for spatial queries
  virtual void interact(Entity* by);
};
```
Future: optional component expansion (HealthComponent, InventoryComponent, PowerPortComponent) without full ECS migration.

## Spatial Representation
- Static vs Dynamic flag needed for broad-phase partition.
- For now AABB via bounds; later quadtree / uniform grid acceleration.

## Interaction Layers
| Layer | Examples | Interaction Rules |
|-------|----------|-------------------|
| Actor | Player, NPC, Hostile | Collide, pathfind, exchange damage |
| Flora | Crop, Tree | Tool actions, harvest |
| Infrastructure | Rail, Pylon | Placement adjacency validation |
| Item | ItemEntity | Pickup, decay timer |
| Ritual | Altar | Activation gating |
| Hidden | HiddenLocation | Discovery triggers |
| Effect | Projectile | Collision -> damage / status |

## Damage & Status Roadmap
Introduce typed damage (Physical, Arc, Corrupt, Pure). Entities expose resistance table. Status effects (Burn, Chill, Corrupt Bloom) apply DOT or slow. Ritual zones can cleanse or amplify statuses.

## Persistence Strategy
Each entity serializes minimal schema:
```
{ type: "Rail", pos: [x,y], data: {...} }
```
Version field for migration. Use registry mapping type string → factory.

## Event Hooks
- OnSpawn / OnDespawn
- OnDiscovery (hidden artifacts)
- OnDamage(source, amount, type)
- OnDeath(source)
- OnActivate (ritual constructs)

## Hidden Entities
HiddenLocation uses map coordinate trigger; on discovery spawns item (dongle) and logs codex entry.

## Performance Considerations
- Bucket entities by update frequency (fast: projectiles; medium: actors; slow: environmental growth) to reduce per-frame cost.
- Late: multi-thread background simulation slices (power, water) producing diff applied on main thread.

## Road to Componentization
Define optional mixins: `HasHealth`, `HasInventory`, `HasPowerPort`. Composition via CRTP or delegated member objects to avoid virtual explosion.
