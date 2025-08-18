# TESTME: Manual & Future Automated Test Guide

## Purpose
Living checklist for verifying core systems after changes. Convert into automated harness progressively.

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

## Targeted Edge Tests
- Planting fails gracefully if no seeds present (log message) – no crash.
- Attempt planting on non-plantable tile – refused.
- Projectile fired with no movement input uses default rightward direction.
- Save while dead: After load, dead state restored & respawn timer resets.
- Minimap bounds: Player near edges does not render viewport rectangle outside map.

## Data Integrity Checks (Future Automation)
- JSON round-trip: save -> load -> save produces semantically equivalent structures (allow ordering differences).
- Crops: after growth completion and removal, tile flagged Plantable again.
- Inventory: serialization preserves counts & order (except removed empty stacks).
- Soil arrays: lengths == width*height; values clamped within [0,1].

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
