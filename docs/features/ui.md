# UI / UX Design

## Principles
- Clarity under layered simulation (progress at a glance).
- Low modal friction; panels slide rather than fully occlude world.
- Diegetic flavor: subtle glyph motifs for transcendence layers.

## Core Panels
- Inventory (grid w/ stack counts, rarity borders)
- Crafting / Processing (recipes filter by tag, shows delta projections)
- Quest Journal & Directives
- Power Grid Overlay (toggle): node icons + flow pulses
- Water & Soil Overlay: fertility, moisture heatmaps (cycle with key)
- Ritual Configuration Panel

## HUD Elements
- Health bar (player & targeted hostile mini-bar)
- Status effect icons row
- Clock & Weather indicator
- Resource Meters (resonance, corruption) appear contextually near altar events
- Mini notifications (harvest streak, mutation discovered)

## Interaction Patterns
- Hover tooltip: item stats, mutation traits, power node efficiency.
- Drag & drop: inventory ↔ cart ↔ loader UI.
- Shift-click: quick transfer to context inventory.
- Ctrl-hover: advanced telemetry (debug mode toggle).

## Feedback Effects
- Harvest: pop number + bloom particle + soft pluck sound.
- Level / Trait unlock: screen vignette pulse, layered chord.
- Rail congestion warning: segment glow + subtle ticking.

## Accessibility
- Scalable UI (0.75x–1.5x) via settings.
- Colorblind-friendly palettes for overlays (distinct luminance ranges).
- Input remap screen (JSON persistence).

## Data-Driven Layout
UI JSON defines panel rectangles, anchor rules, hotkeys. Hot reload for iteration.

## Widget Style Tokens
```
{ font:"assets/fonts/arial.ttf", cornerRadius:4, border: #334, panelBg: rgba(20,24,28,215) }
```

## Performance Considerations
- Batch sprite text glyph caching.
- Dirty flag redraw for static panels (quest list).

## Future Enhancements
- Minimap with discovery fog.
- In-world labels projected for key structures.
- Gesture wheel for quick tool selection.
