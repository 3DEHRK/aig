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

🍌 Water/Fertilize All Test Protocol
1. Position player near mixed plantable tiles (some empty, some with crops).
2. Trigger batch water (Help key) -> console logs number of tiles affected; moisture overlay should brighten those tiles.
3. Trigger batch fertilize (ToggleDeathPenalty key) -> fertility overlay increases intensity for affected tiles.
4. Mature (finished) or currently growing crops are skipped (verify tile moisture changes only on empty soil for now due to heuristic).
5. Cooldown prevents immediate re-use (<2s); attempt early use logs no action.

🍌 Global Buff System Test Protocol
1. Press Journal key to add moisture decay reduction buff.
2. Buff list appears under Buffs header with countdown ~120s.
3. Observe soil moisture decay slower (compare tile moisture after fixed time vs baseline before buff).
4. After duration expires, buff entry disappears and decay rate returns to normal.

🍌 Contracts Board Test Protocol
1. Ensure `data/contracts.json` exists with sample contracts.
2. Start game; console logs: [Contracts] Loaded N contracts.
3. Press Contracts key binding (configure action "Contracts") to toggle panel; list shows each contract with inputs and rewards.
4. Give player required input items (ensure starter seeds/items cover requirements) and verify quantities.
5. When inventory satisfies a contract, it auto-completes within a frame; dialog popup: "Contract completed: <Name>".
6. Inputs deducted from inventory; rewards appear as new items (stack counts increment).
7. Panel updates to show [Done] status; cannot be completed again.
8. Save & reload: completed status persists (future persistence extension if implemented).

🍌 Trader System Test Protocol
1. Ensure `data/trades.json` exists and console logs: [Trader] Loaded X trade offers on start.
2. Toggle trader panel with Trader action key (bind key in input mapping if not present) and verify list displays each offer with index.
3. Spawn or collect required input items (e.g., 5x fiber) in inventory.
4. Trigger trade (future: press numeric key). For now manually call `tryExecuteTrade(0)` hook or simulate by adding key binding later.
5. Verify inventory: input items decremented, reward items added; console log confirms.
6. Attempt trade without enough items -> log warns and no inventory change.

🍌 Unlock Rules Test Protocol
1. Harvest a crop (e.g., wheat). Observe dialog: New seed unlocked: seed_wheat.
2. Inventory now contains at least one new seed item granted automatically.
3. Plant newly unlocked seed to verify normal growth.
4. Place first rail segment; dialog appears about biome seeds unlock (placeholder message).
5. Additional wheat harvests do not repeat unlock dialog (idempotent).

🍌 Day/Night Cycle Test Protocol
1. Observe ambient color shift over ~10 minutes full cycle (default 600s). For faster test temporarily set dayLength small.
2. At t≈day (0.25) ambient nearly transparent; at night (0.75) dark blue overlay.
3. Lamps render warm glows visible more strongly at night (contrast higher).
4. Verify lighting overlay follows camera when moving.
5. Confirm performance acceptable (no noticeable frame drop with few lamps).

🍌 Micro-Wind Crop Shader Test Protocol
1. Observe crops gently swaying horizontally (few pixels) even at early stage.
2. Sway amplitude scales with growth stage (mature crops move more).
3. Amplitude modulates over time (notice periodic strengthening about every few seconds).
4. During dusk/night segment (timeOfDay 0.5–0.85) sway slightly stronger (~20%).
5. Harvested or withered crops stop swaying.

🍌 SFX / Ambience Test Protocol
1. Move player continuously; every ~0.4s (walking speed) a footstep sound plays. Vary direction; interval shortens slightly when sprinting (if speed buffs applied later) and lengthens when nearly idle.
2. Walk across soil tile (Plantable) vs empty/grass vs rail: hear distinct pitch/volume differences (soil slightly louder, rail slightly metallic).
3. Stand still: footstep sounds cease after partial timer decay; no idle looping.
4. Wait ~9s: a random ambient one-shot (wind / bird / leaves) plays at moderate volume. Over several minutes, distribution appears varied (not repeating the same sound every time).
5. Confirm ambient interval randomization (log, or measure time between 3 consecutive cues; variation ±30%).
6. Continue moving during ambient playback; footsteps still audible and not suppressed.
7. Toggle day/night progression (fast-forward timeOfDay if needed) – ambient triggers irrespective of time (future: vary by time, acceptable now).
8. Ensure no crash if placeholder .ogg files missing (SoundManager silently ignores failures).
