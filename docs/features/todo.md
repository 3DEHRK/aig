# TODO / Implementation Roadmap

## Completed (Moved from TODO)
- Minimap with fog-of-war, scaling (J), view rectangle (V), entity icons (G)
- Respawn navigation aids (marker P, distance O, unit toggle T, arrow)
- Death penalty toggle (Y) with inventory stack reduction (exempt seeds)
- Help / controls overlay (H) with persistence
- Persistence for UI & minimap preferences (minimap scale, view rect, entity icons, help overlay)

## Phase 0: Stabilize Core
- [ ] Refine entity architecture (helpers for collision, health mixin)
- [ ] Complete save/load coverage (altar state, rail meta, dialog state)
- [ ] Input remapping layer
- [ ] Headless test mode & input replay harness

## Phase 1: Farming Expansion
- [ ] Soil → crop growth influence (moisture & fertility curve)
- [ ] Growth algorithm with stress & mutation hooks
- [ ] Watering tool item (vs bare right-click) + moisture decay balancing
- [ ] Basic fertilizer item & effect tiers

## Phase 2: Logistics & Power Foundations
- [ ] Track grid data serialization (beyond visual rails)
- [ ] Cart entity pathing & route editor UI
- [ ] Loader / unloader interaction with inventory
- [ ] Power node registry & fixed-timestep simulation loop
- [ ] Generator + storage + consumer minimal set

## Phase 3: Hostiles & Ecology Pressure
- [ ] Threat level computation (farming intensity, pollution)
- [ ] Hostile spawn scheduler (scaling waves)
- [ ] Additional hostile archetypes w/ differentiated AI
- [ ] Damage types groundwork (resistances framework)

## Phase 4: Quests & Directives
- [ ] Directive evaluator engine
- [ ] Quest JSON loader & condition factory
- [ ] Journal UI panel

## Phase 5: Ritual & Realm
- [ ] Altar upgrade tiers
- [ ] Hidden realm instance preservation & restoration
- [ ] Portal visual / particle system
- [ ] Realm instability index & wraith spawns

## Phase 6: Polishing & Feedback
- [ ] Particle system module (harvest, hit, portal)
- [ ] Layered audio mixing (ambient + events)
- [ ] Screen shake / camera easing utilities
- [ ] HUD expansions (power, moisture trend graphs)

## Phase 7: Optimization & Refactor
- [ ] Spatial partition (uniform grid) for broad-phase queries
- [ ] Batch rendering (sprite atlas / instancing path)
- [ ] Data-driven tunables file (JSON) for balancing

## Stretch Goals
- [ ] Seasonal waveform system
- [ ] Taming / conversion of constructs
- [ ] Quantum logistics tier
- [ ] Ascension meta modifiers cycle
