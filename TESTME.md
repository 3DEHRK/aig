# TESTME: Manual & Future Automated Test Guide

## Purpose
Living checklist for verifying core systems after changes. Convert into automated harness progressively.


🍌 Plant-System Test Protocol
1. Launch game; verify console logs: [CropConfig] Loaded X configs.
2. Ensure player starts with seed_wheat items (inventory size increases by 5 entries).
3. Observe existing spawned wheat crops progress through stages (size & color changes) over time matching stage_durations.
4. Adjust soil moisture (simulate by waiting) below death threshold (<0.15) for prolonged period; confirm wither log and visual brown tint.
5. Harvest at final stage: interact and check console log 'Crop harvested' includes yield and quality.
6. Fertility decreases at harvested tile (compare fertility before/after via potential debug overlay).
7. Save/load (if implemented) retains growth stage, withered/harvested state, yield, quality.

🍌 Hold-to-Harvest Test Protocol
1. Grow at least one mature crop (or wait for existing ones).
2. Hold Interact key over a mature crop; confirm harvest triggers after ≤0.12s slice.
3. Continue holding and move cursor; adjacent mature crops (within 3x3) harvest in successive intervals without releasing key.
4. Verify withered or already harvested crops are skipped.
5. Release Interact; hold system stops (no further harvesting).

🍌 Magnet Pickup Test Protocol
1. Drop several items (spawn ItemEntity instances) at varying distances (1–3 tiles) from player.
2. Approach until within 2.5 tile radius; items begin drifting toward player (magnetizing state).
3. Items auto-collect when within ~28 px (inventory count increases, console pickup log).
4. Ensure items outside radius remain static until entering the radius.
5. Inventory full condition (fill inventory) prevents collection and logs refusal.

🍌 HarvestFX System Test Protocol
1. Harvest a mature crop; observe squash (slight compression) then pop expansion then shrink/fade.
2. Confirm "+N" yield text appears during pop phase briefly.
3. Screen shake (vertical jitter) occurs only during pop phase; amplitude ≤3 px.
4. Effect fully removed after ~0.45s.
5. Multiple crops harvested rapidly spawn overlapping FX independently.
