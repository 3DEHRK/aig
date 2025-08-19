# TESTME: Manual & Future Automated Test Guide

## Purpose
Living checklist for verifying core systems after changes. Convert into automated harness progressively.

### Directive Persistence (Phase 4 Update)
1. Start new session; plant a seed to complete Plant a seed directive; harvest 2 crops (progress 2/5); place one rail (complete build_rail).
2. QuickSave (K). Inspect save JSON: directives array with three entries (ids plant_seed, harvest_crops, build_rail). Verify progress & satisfied flags reflect in-game state.
3. Quit & relaunch; QuickLoad (L). Journal (press J) shows completed directives with [Done] plus partially progressed harvest entry (2/5).
4. Harvest remaining crops to reach 5; directive auto-updates to done; save & reload; all three directives satisfied.
5. Delete directives block from save JSON manually; reload; defaults seeded (all unsatisfied) without crash.
6. Corrupt one directive entry (remove text) and reload; fallback text restored for known id.

### Journal Toggle Binding
1. Press J (default Journal action) to open panel; verify directives & quests listed.
2. Rebind Journal in bindings.json (e.g., to K) with {"keys": {"Journal": <code>}}; launch; J no longer toggles; new key does.
3. QuickSave to persist remap (bindings.saved.json) then relaunch without original bindings.json; Journal key still remapped.

### Cart Route Assignment UI (New)
1. Press Z to toggle Cart Route Mode (status message bottom of screen).
2. Left-click each rail tile of a desired path: numbered waypoint markers (0,1,2,...) appear with cyan circles; active/current waypoint highlighted amber.
3. Waypoint connection lines render between sequential points; loop line (fainter) drawn from last to first when 3+ points.
4. Cart immediately follows new path (after reaching or snapping to first waypoint if list cleared & rebuilt).
5. Right-click while in route mode: clears all waypoints; cart stops (no movement until at least one waypoint added).
6. Toggle mode off (Z) hides overlay; cart continues along existing path.
7. Save/Load currently does NOT persist cart waypoints (known limitation) – verify they are lost after reload (future task).
8. Attempt to add waypoint on non-rail tile: ignored (no crash, no marker).
9. Regression: Exiting route mode re-enables normal left-click interactions (NPC dialog, altar activation) and right-click watering.

### Damage Hook (Updated)
1. Fire projectile at hostile; verify damage number shows and health decrements once.
2. Instrument HostileNPC::onDamaged and Player::onDamaged to log; confirm logs on each hit (inline takeDamage calls onDamaged).
3. Multiple simultaneous hits still produce one log per projectile.
4. Future: add visual flash effect and sound trigger inside onDamaged.
5. Regression: ensure removing inline definition (if moved later) does not reintroduce duplicate out-of-line definitions.
6. Player invulnerability: while invulnerable, takeDamage early-returns so onDamaged not called (verify by logging when invuln active).

### Entity Health Interface (New)
1. Spawn into game; damage hostile with projectiles until log shows death; ensure HostileNPC removed and no crash when projectiles still exist.
2. Confirm player death sequence still triggers using unified health interface (internal calls rely on getHealth/getMaxHealth).
3. Future automation: iterate all entities; if hasHealth()==true then 0 < getHealth() <= getMaxHealth() unless isDead()==true.

### Input Remapping (New)
1. Edit (or create) bindings JSON (future) to map MoveUp to a non-default key; start game; confirm movement uses new key.
2. Rebind Interact to a different key; ensure planting/interactions trigger only on new binding.
3. Regression: default keys still work when JSON absent.
4. Headless plan: feed synthetic key events mapping actions to ensure deterministic movement path.
5. Create bindings.json with:
```
{
  "keys": { "MoveUp": 25 }
}
```
(where 25 corresponds to e.g. sf::Keyboard::Key::W or change to another code) then launch; verify movement uses action mapping once integrated.
6. Delete/rename bindings.json; launch game; confirm default WASD + E/Space still function (fallback applied).

### Collision Helpers (New)
1. Move player against solid boundary; ensure no jitter or penetration (resolveAxis logic).
2. Slide along wall diagonally: Y constrained while X continues (or vice versa) without sticking.
3. Regression: No regression in rail placement or hostile pathing due to helper additions.

### Item Registry (New)
1. Launch game: console should log number of loaded item defs.
2. Remove an item entry from JSON; restart; inventory seeds fallback keep id as name if missing.
3. Corrupt JSON (syntax error) -> game continues without crash; fallback items created.
4. Add new item id and call addItemById via debug/inventory; verify name & desc populate.

### Tunables JSON (New)
1. Edit player.speed in data/tunables.json (e.g. 260) then relaunch; verify speed updated once hooked (future task).
2. Corrupt file (truncate) -> loader logs failure but game runs.
3. Add new farming base growth key; ensure parser still loads.

## Quick Start Regression (Manual)
1. Launch game; ensure window opens without errors.
2. Move player (WASD / arrow keys if supported) – collision blocks against solids.
3. Press I: Inventory toggles open/close.
4. Press H: Help overlay appears/disappears.
5. Planting:
   - Ensure at least one `seed_wheat` in inventory at start.
   - Move near a plantable tile (brown). Press E (no entity targeted) -> crop appears; inventory seed count decrements.
6. Growth:
   - Wait (or speed modify if debug) until crop stage advances; upon completion & harvest logic (E), crop removes & tile returns plantable.
7. Moisture/Fertility:
   - Toggle M/N overlays; right‑click to water hovered tile (moisture alpha increases). Press F to fertilize player tile (fertility tint changes).
8. Combat:
   - Locate hostile (red). Shoot with Space; floating damage numbers appear; hostile disappears on death.
9. Death / Respawn:
   - Purposely receive damage (stand near hostile) until death screen shows; countdown ends; player respawns at last altar or initial point.
   - If Y toggled ON (death penalty), verify non-seed stacks lose ~10% (≥1) on death.
10. Respawn Aids:
    - Toggle P (marker), O (distance text), T (units) and verify arrow & text when respawn is off-screen.
11. Minimap:
    - Toggle U (show/hide). Press J to cycle tile scale 2→3→4→2.
    - Toggle V (viewport rect) & G (entity icons) on/off.
    - Explore new area (move) – fog reveals correctly.
12. Save/Load:
    - Arrange unique state (crop planted mid-growth, hostile damaged, toggles changed, minimap scale altered).
    - Press K (save). Quit application. Relaunch & press L (load). Verify restored state (player pos, health, minimap prefs, inventory, crops, hostiles health, soil stats).
13. Rail Tool:
    - Press B. Left-click plantable tiles to add/remove rails. Exit tool (B) then save (K) & reload (L) to ensure rails persist via map tile serialization.

### Altar Persistence (New)
1. Locate altar; ensure not active (no prior activation message in log).
2. Give player required item IDs (add via inventory JSON edit or spawn) e.g., `dongle_mysterious`.
3. Interact (E) near altar: log shows activation. Save (K).
4. Quit & relaunch; Load (L). Verify altar remains active (interaction logs “already active”), respawn set if previously assigned.
5. Confirm `altars` array in save JSON has active=true and required items list.

### Dialog Persistence (New)
1. Trigger an NPC dialog (click NPC). Advance one line (E or Space) leaving remaining lines.
2. Save (K) while dialog still active.
3. Quit & relaunch; Load (L). Dialog box should reappear with remaining lines starting from the next unread line.
4. Advance through remaining lines; dialog closes properly; subsequent save omits dialog block or marks inactive.
5. Verify save JSON contains `"dialog": { "active": true, "lines": [...] }` when mid-dialog; absent or `active:false` otherwise.

### Rail Metadata Persistence (New)
1. Enable rail tool (B). Place a line, a corner, and a T-junction of rails.
2. Observe color differences (indicates connection count 1..4).
3. Save (K). Inspect save JSON: ensure `railMeta` array present and length == w*h.
4. Remove a middle segment to split a line; colors update (fewer connections).
5. Save & reload; verify updated connection colors persist.
6. Load an older save (without `railMeta`) still renders correct connections (bitfields recomputed).

### Hit Flash Feedback (New)
1. Damage hostile: observe brief white flash tint fading back to red within ~0.15s.
2. Player takes damage: sprite shifts toward yellow then returns to green over 0.2s (unless invulnerable flicker overrides).
3. Rapid successive hits reset flash timer (sustain brighter tint while taking continuous damage).
4. Regression: flash does not persist indefinitely; after timer colors revert exactly to baseline.

### Projectile Knockback & Lifetime (New)
1. Set projectile.knockback to a noticeable value (e.g., 80) in tunables and rebuild; shoot hostile: observe slight displacement away along projectile travel direction.
2. Reduce knockback to 0; rebuild; confirm no displacement.
3. Adjust projectile.lifetime to 0.5; fire and visually confirm projectile disappears sooner; set to 5.0 and confirm longer travel (bounded by screen).
4. Verify remainingLife() decreases to 0 and expired() returns true when projectile removed (instrument logs if needed).
5. Regression: knockback scaling applies direction from projectile velocity (not player movement when stationary).

### Hostile Rage (New)
1. With default rage_speed_mult (1.4) and rage_duration (1.5) tunables, damage hostile once; during rage window observe faster chase speed (measure distance closed in 1s vs pre-hit baseline).
2. Set rage_speed_mult to 2.0; rebuild; hostile burst speed clearly higher.
3. Set rage_duration to 0; rebuild; hostile no longer speeds up after damage (rageTimer never active).
4. Multiple hits during rage refresh rageTimer (stay in boosted state as expected).
5. Regression: After rageTimer expires, speed returns exactly to base (no lingering multiplier).

### Hostile Spawn Timer & Zones (New)
1. Start game; note initial hostile count (should increment over time up to maxHostiles=6).
2. Wait > spawn interval (8s) repeatedly; verify new hostile appears if below cap.
3. Lure player near a spawn point; ensure that point does not spawn (distance < 300 units) — hostiles still spawn from other distant points.
4. Reduce maxHostiles in code (e.g., to 2) and relaunch; count never exceeds 2.
5. Increase hostileSpawnInterval (e.g., to 2.f) for rapid test; hostiles arrive faster but still respect cap.
6. Kill hostiles (projectiles) and observe spawning resumes until cap reached.
7. Regression: No spawn occurs inside player view unexpectedly close (< minSpawnDistance) when enough distant points exist.

### Threat Scaling Placeholder (New)
1. At game start, record hostileSpawnInterval (~8s) and maxHostiles (6).
2. Move continuously for 2 minutes; observe interval decreases (approx down toward 4s at high threat) and maxHostiles increases up to 10.
3. Standing still: threat increases slowly (time component only) – verify slower rise.
4. Confirm threatLevel caps at 1.0 (no further interval reduction beyond intended min).
5. Regression: Resetting game (new session) resets threatLevel to 0; threat not persisted (intended for now).
6. Optional: Temporarily log threatLevel each 10s to verify growth curve stability.

### Hostile Variants (New)
1. Start new session; observe initial spawned hostile near (400,300) is a grunt (small red square).
2. Wait for additional spawns; early threatLevel low so tankSpawnChance ~0 -> only grunts appear.
3. Increase threat (move continuously ~60s); verify occasional larger darker red square (40x40) appears = Tank variant.
4. Tank has noticeably slower speed (approx 40 vs 70) and higher health (3x grunt). Count projectile hits required.
5. Confirm spawn ratio approaches 50% maximum at very high threat (simulate by temporarily forcing tankSpawnChance=0.5 in code for test).
6. Kill both variants; loot drop probabilities unchanged (fiber ~60%, crystal ~10%).
7. Save/Load: variant type now persisted (type: "grunt"|"tank" in hostiles array); after reload, sizes/health reflect stored type.
8. Regression: ensure spawnHostile uses deterministic RNG sequence (seed 1337) for reproducible automated tests (first 10 spawn types stable after fixed movement script).

## New Fallback Test
- Temporarily remove or rename `assets/textures/entities/altar.png`.
- Launch game: verify no crash; console logs fallback message; altar rendered as purple outlined square.
- Restore texture: altar uses sprite again (ensure cache invalidation by changing filename or clearing build if needed).

## Fallback Texture Test
1. Rename an existing texture path (e.g., altar) so load fails.
2. Launch: console logs one missing texture message; placeholder magenta checker appears; no crash.
3. Restore texture; relaunch; real sprite appears.

## Targeted Edge Tests
- Planting fails gracefully if no seeds present (log message) – no crash.
- Attempt planting on non-plantable tile – refused.
- Projectile fired with no movement input uses default rightward direction.
- Save while dead: After load, dead state restored & respawn timer resets.
- Minimap bounds: Player near edges does not render viewport rectangle outside map.

## Edge Cases
- Loading save without `altars` key: no crash.
- Save with multiple altars (future) retains list.

## Data Integrity Checks (Future Automation)
- JSON round-trip: save -> load -> save produces semantically equivalent structures (allow ordering differences).
- Crops: after growth completion and removal, tile flagged Plantable again.
- Inventory: serialization preserves counts & order (except removed empty stacks).
- Soil arrays: lengths == width*height; values clamped within [0,1].
- Validate at least one active altar sets respawnPos inside map bounds.

## Planned Automated Harness Tasks
- Headless build flag `BUILD_HEADLESS` to instantiate PlayState without window & run fixed steps.
- Scripted input queue feeding `InputManager` to replicate manual test sequence.
- Assertion macros for invariants: entity bounds inside world, health within range, no duplicate rail tiles.
- Snapshot comparator: hash of serialized JSON vs expected baseline per scenario.

## Performance Smoke
- Measure frame time after spawning N hostiles (future) – ensure projectile loop remains performant (<2 ms on baseline machine for 50 hostiles).

## Regression Checklist (Tick each before commit)
- [ ] Build succeeds (Debug/Release)
- [ ] All manual quick start steps pass
- [ ] Save/Load state parity
- [ ] No unintentional console spam/exceptions
- [ ] README & docs updated for new feature flags

## Notes
Update this file whenever a new feature adds input, persistence keys, or systemic interaction requiring verification.

# TEST / Verification Scenarios
## Headless deterministic tick stepping
1. Build headless target.
2. Run with `--headless --ticks 120` and capture JSON output.
3. Ensure `ticks_ran` == 120 and status ok.
4. (Future) Compare snapshot hash of world state after fixed ticks for regression.
## Tunables application
1. Open data/tunables.json and change player.speed to a distinct value (e.g., 300).
2. Run game; measure player movement distance over 1 second (rough visual) vs previous build (should increase proportionally).
3. Change hostile.grunt.health to 20; rebuild; start game and shoot hostile counting hits (projectile damage constant) until death; hits required should change accordingly.
4. Verify player regen parameters adjust when regen_rate or regen_delay modified (observe time to start healing in log or visually).
5. Set player.regen_curve_exponent to 2.0; damage player to 10% HP then observe regen speed initially high then taper as health approaches max (compare time per 10 HP segment).
6. Adjust soil.moisture_target to 0.6 and moisture_decay_per_sec to 0.05; water a tile to near 1.0 then measure time to drift toward 0.6 vs default config (should stabilize around new target faster).
7. Set soil.fertility_target to 0.8 and fertility_regen_per_sec to 0.02; deplete a tile fertility (simulate via future mechanic or temporary code) then observe faster regeneration toward higher target.
8. Increase projectile.speed to 500; verify projectile travel distance per frame increases and hits hostiles sooner (time to impact lower).
9. Reduce hostile.grunt.contact_damage to 1; stand in attack range and measure slower health loss vs default.
## Quit behavior
1. Launch game and press ESC -> window should close cleanly.
2. Relaunch and click window close X -> window should close.
3. Confirm no crash or hang on exit.
## Inventory action mapping
1. Modify bindings.json to remap Inventory action to a new key (e.g., O) with {"keys": {"Inventory": <keycode>}}.
2. Launch game; press old key I (should no longer toggle).
3. Press new key (O) and confirm inventory UI toggles.
## Collision resolution
1. Move player against corner of two solid tiles; ensure sliding along wall feels smooth (no sticking or diagonal jitter).
2. Attempt to move diagonally into a tight gap one tile wide; player should stop cleanly without penetrating.
## Action mapping: Shoot & Interact
1. Remap Shoot action to another key in bindings.json; confirm projectile still fires with new key and not with Space.
2. Remap Interact to another key; verify planting and NPC dialog still function with remapped key.
## Unified action mapping toggles
1. Remap ToggleMoisture (M) to another key and verify overlay toggles only on new key.
2. Remap CycleMinimapScale (J) and ensure scale cycles correctly.
3. Remap QuickSave / QuickLoad; verify save/load still work.
4. Remap Quit to non-Escape key; confirm both Escape (if unbound) stops working and new key closes game.
## Farming tunables influence
1. Note time for a wheat crop to reach harvest with default tunables.
2. Set farming.moisture_factor to 2.0 and fertility_factor to 2.0, rebuild, plant new crop; growth should noticeably accelerate (roughly ~4x component effect depending on moisture/fertility values).
3. Set both factors to 0.5; growth should slow accordingly.
## Projectile damage tunable
1. Set player.base_damage in tunables.json to a higher value (e.g., 20).
2. Rebuild and count shots required to kill a grunt (health from tunables). Should decrease proportionally vs previous base_damage.
3. Lower base_damage to 2; shots required should increase.
## Binding persistence
1. Rebind Inventory to O via bindings.json or runtime editing (future UI).
2. Press QuickSave (K) to trigger SaveCustomBindings.
3. Exit game, delete/rename original bindings.json, restart.
4. Verify Inventory still mapped to O via bindings.saved.json load.

### Save/Load Hostile Health Regression
1. Damage a hostile, QuickSave (K), quit, relaunch, QuickLoad (L). Confirm hostile health restored (requires setHealth present).
2. Remove `health` key manually from hostiles entry in save JSON, reload; hostile should spawn at full health (maxHealth from tunables).

### Binding Persistence Regression
1. Rebind an action and QuickSave (K); ensure `bindings.saved.json` created with updated mapping (SaveCustomBindings visible again after include fix).

## Save/Load Coverage Audit (Phase 0)
- Player: position, health, dead flag, respawn position, overlays (respawn marker/distance, minimap toggles, death penalty, help), minimap scale, inventory ✅
- Crops: type, growth state, position ✅
- Map: tiles, soilMoisture, soilFertility, explored, railMeta ✅
- Rails: reconstructed from map ✅
- Hostiles: position (center), health ✅ (no type differentiation yet)
- Altars: position, active, required items ✅
- Dialog: active + remaining lines ✅ (minimal)
- Not Saved Yet (acceptable for now): projectile list (ephemeral), rail tool enabled flag, overlays moisture/fertility (toggle states stored via player flags), player regen timers, hostile attack cooldowns, soil tunables (loaded from data), bindings (separate persistence), damage accumulated stats.
- Future Consideration: store player baseDamage if tunable changes mid-save, hostile variant type when multiple types exist.

### Phase 0 Completion Verification (New)
1. Entity Architecture: Confirm projectile knockback never embeds hostiles inside solids (observe after multiple hits against wall). Hostile nudge stops when solid tile encountered.
2. Collision Helpers: Move diagonally into walls; sliding works without jitter; hostile knockback uses axis clamp (no tunneling).
3. Save/Load Coverage: After complex state (rails, overlays toggled, fertilizer unlocked, crops mid-growth, hostiles damaged), save & reload; JSON contains railMeta, overlays flags, harvested_crops, fertilizer_unlocked, hostile health values.
4. Input Remapping: Modify bindings.json remapping at least three actions; launch; confirm new keys active and SaveCustomBindings persists them.
5. Tunables Application: Change projectile.speed, hostile.grunt.rage_speed_mult, farming.base_growth_seconds.wheat; relaunch; each change reflected in runtime (measure movement/growth/delay).
6. Regression: No missing-field crashes when loading older save missing railMeta or fertilizer fields (defaults applied gracefully).

## Performance Tests
### Hostile Spawn Performance
1. Start game with clean save.
2. Wait for hostiles to spawn (up to maxHostiles).
3. Measure frame time for 10 seconds; ensure average frame time < 16ms (for 60 FPS target).
4. Increase maxHostiles to 20; relaunch; verify frame time remains stable.
5. Damage hostiles to trigger death; ensure respawned hostiles do not cause frame time spikes.

### Projectile Performance
1. Set projectile.speed to 1000 in tunables.json.
2. Fire projectiles rapidly at a wall; observe no frame drops or stutter.
3. Reduce speed to 100; verify projectiles still render and collide correctly without performance loss.
4. Test with large numbers of projectiles (e.g., 100); ensure no significant impact on frame time.

### Hostile Pathfinding Performance
1. Spawn multiple hostiles (e.g., 10) and observe initial pathfinding.
2. Measure time taken for all hostiles to reach player; ensure within acceptable limits (e.g., <500ms).
3. Increase complexity: add obstacles or change player position frequently; verify hostiles adapt without excessive delay.
4. Test edge case: lure hostiles to map edge; ensure they do not get stuck or cause performance issues.

### Save/Load Performance
1. Create a complex game state (many hostiles, crops, items).
2. Save game and measure time taken.
3. Reload the save and measure time taken; ensure both times are within acceptable limits (e.g., <2 seconds).
4. Test with corrupted save (e.g., missing fields); verify game handles load gracefully without crash.

### Regression: Performance Baseline
1. Compare current performance metrics against baseline (from earlier commit or version).
2. Ensure no new performance issues introduced (e.g., frame time spikes, increased save/load time).
3. Pay attention to areas with recent changes (e.g., new features, modified systems).

## Future Considerations
- Automated tests for all manual regression steps.
- Continuous integration setup to run tests on each commit.
- Performance benchmarks for critical systems (e.g., rendering, physics, AI).

# TESTME: Loot Drop Verification Scenarios
### Hostile Loot Drop Stub (New)
1. Kill 20 hostiles; drops deterministic sequence (seed 1337). Expect exact pattern stable between runs (document first 10 results after one session for regression).
2. Fiber drops probability target 60%; crystal 10% (tune later). Sequence reproducible.
3. Verify items spawn at hostile center offset and can be collected.
4. Regression: RNG deterministic ensures automated test numeric counts stable; change seed only if updating baseline.
5. Save immediately after drops on ground; reload; confirm persistence plan (currently ephemeral if ItemEntity not serialized; add later if needed).

### Regression: Inventory Toggle & Planting (Fixed)
1. Press I: Inventory UI appears; press I again: closes (toggle).
2. Rebind Inventory to O in bindings.json; launch; I does nothing, O toggles UI.
3. With at least one seed_wheat, stand near plantable soil, press E with no entity targeted: crop plants, seed count decrements, tile becomes non-plantable.
4. Attempt planting when no plantable tiles in radius: no crop created, no seed consumed.
5. Ensure planting still works after opening/closing inventory and after saving/loading.

### Inventory Item Visibility (Fixed)
1. Open inventory (I); each slot containing an item displays 3-char name prefix and stack count if >1.
2. Add multiple items (different names); verify distinct prefixes visible.
3. Large stack (>=10) shows full numeric count (e.g., ":12").
4. Remove items until slot empty; slot shows only background rectangle.
5. Regression: Text remains legible over dark background; adjust color if future theme changes reduce contrast.

### Farming Onboarding Demo (New)
1. New session start: dialog appears with planting instructions; three adjacent soil tiles present near spawn.
2. Plant seed on one demo tile: dialog advances to watering step.
3. Water tile (Right Click): dialog advances to fertilizer/harvest step.
4. Optionally fertilize (F) then wait for growth; harvest triggers completion dialog.
5. After final dialog closes demo flags set (no re-trigger on further actions).
6. Save & reload mid-demo: progression persists (verify dialog resumes correct step or note if not yet persisted—persistence future enhancement).
7. Regression: Demo tiles revert to normal gameplay after completion (no repeated guidance spam).

### Dialog Screen-Space Anchoring (Fixed)
1. Start game; trigger NPC dialog (click NPC).
2. Move player while dialog open: panel remains fixed near bottom of screen (does not scroll with world).
3. Resize window (if supported): dialog re-centers / width adjusts within new window bounds (40px margin each side).
4. Open inventory simultaneously: both inventory UI and dialog visible without overlapping critical text (adjust stacking later if needed).
5. Regression: Ensure dialog closes properly after last line and no world-space version left behind.

### Loot Variety (Updated)
1. Kill 30 hostiles; count fiber vs crystal_raw drops. Fiber should approximate 60% occurrence, crystal_raw roughly 10% (allow small variance). Record counts.
2. Verify spawned ItemEntity ids exactly "fiber" and "crystal_raw" matching items_basic.json definitions.
3. Collect drops; inventory stacks increase accordingly (fiber stacks, crystals accumulate individually or stack if implemented).
4. Save after several drops on ground; reload (drops currently ephemeral if ItemEntity not serialized—note limitation if still true).
5. Regression: No old ids (fiber_common, crystal_rare) appear in newly spawned loot.

### Healing Salve Consumable (New)
1. Accumulate >=3 fiber drops.
2. Press bound CraftSalve key (default Q) -> log "Crafted small healing salve" and inventory gains salve_small.
3. Damage player below full health; press UseSalve key (default R) -> health increases by up to 25 (clamped) and salve stack decrements.
4. Attempt crafting with <3 fiber -> log informs remaining needed.
5. Attempt use with no salve_small -> no effect, no crash.
6. Regression: Craft multiple times; stacking salves works (each use removes one).

### Rail Orientation Metadata (Completed)
1. Place straight, corner, T-junction rails using Rail Tool (B).
2. Toggle rail overlay (X) – connection lines and coloring reflect exits.
3. Save (K) and inspect JSON: railMeta present; length width*height.
4. Remove a rail, save, reload: orientation updates persist.
5. Older saves (without railMeta) still compute connections (backwards compatibility).

### Cart Persistence (Planned)
1. Current limitation: cart waypoints, loader/unloader tiles NOT yet serialized (verify lost after reload). Will be added before expanding logistics depth.

### Cart Logistics Loader/Unloader (New)
1. Enter Cart Route Mode (Z). Press 1 then left-click a rail tile to set loader (L indicator appears). Press 2 then left-click another rail tile along path to set unloader (U indicator).
2. Ensure player has several seed_wheat in inventory (default start). Allow cart to pass over loader tile; each second near tile cart gains up to 1 seed (tint shifts to cargo color when carrying).
3. Cart reaches unloader tile: seeds transferred back to player inventory one per second; cart tint reverts when empty.
4. Clearing waypoints (right-click) stops movement; logistics pauses (no transfers unless cart within radius already).
5. Remove loader assignment (set loader again elsewhere) – previous tile no longer transfers.
6. Capacity test: After 16 items loaded (capacity), loader stops adding; unloader resumes unloading until empty then loader can refill.
7. Regression: Exiting mode (Z) does not prevent ongoing transfers when cart passes tiles (still active internally).

### Cart Waypoint & Logistics Persistence (New)
1. Enter route mode (Z), create at least 4 waypoints and ensure cart loops.
2. Assign loader (1 + click waypoint 1) and unloader (2 + click waypoint 3).
3. Save (K). Inspect save JSON: has cart object with waypoints array and loop flag; logistics with loader/unloader coords.
4. Quit, relaunch, load (L). Cart immediately follows restored path; loader/unloader functioning without reassign.
5. Clear waypoints, save, reload: cart has zero waypoints (idle) until new ones added.
6. Regression: Older save without cart/logistics keys loads (cart retains default initial demo path or empty if not present) without crash.

### Starter Logistics Quest (New)
1. New session: ensure quest list includes "Automate a Wheat Haul" (console shows nothing yet; verify by inspecting save JSON after first save: quests array with id starter_logistics).
2. Set loader/unloader and run cart so 5 seeds move. After each unload progress increments; when reaching 5 console logs quest completion.
3. Save then reload mid-progress (e.g., after 2 moved); progress persists.
4. After completion, ensure quest marked completed in save (completed:true, objective completed:true, progress==target).

### Quest Chain Progression (Phase 4)
1. Start new session; observe chain hint text: "Chain: Plant a seed".
2. Plant a seed; dialog appears advancing chain to harvest step; hint updates to "Chain: Harvest 5 crops".
3. Harvest 5 crops; chain dialog prompts rail step; hint shows "Chain: Place a rail segment".
4. Place a rail (B then click) or use existing rail tool to add a segment; dialog confirms completion; chain hint disappears (stage 3 done).
5. Save & reload mid-chain (e.g., after planting but before harvest completion); ensure quest_chain_stage persisted and hint reflects correct step.
6. Corrupt or remove quest_chain_stage in save JSON; reload; stage defaults to 0 with seed directive still satisfied if it was (verify no crash).
7. Complete chain fully; save & reload; no chain hint displayed.
