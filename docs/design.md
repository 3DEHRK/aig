Design — Tiles, Textures & Asset List

This document enumerates all tiles, sprites, and texture assets required to implement the systems described in the game design docs. Each asset has recommended sizes, usage, and variant counts. Use consistent naming and folder structure under `assets/textures/`.

Global recommendations
- Tile size: 32x32 pixels (configurable). All tiles align to grid.
- Character sprites: 32x48 per frame with a 3–4 frame walk cycle per direction.
- UI icons: 32x32 or 24x24 depending on UI density.
- Use subfolders: `tiles/`, `entities/`, `items/`, `ui/`, `particles/`, `overlays/`.

Tileset — terrain (tiles/)
- grass_01.png (32x32)
- grass_02.png (variants: 4)
- dirt_01.png
- soil_tilled_01.png
- soil_soaked_01.png (moisture overlay)
- rock_01.png
- water_shallow.png
- water_deep.png
- rail.png (32x32, centered rail piece)
- rail_switch.png
- foundation_wood_01.png

Tile overlays (for dynamic states)
- moisture_overlay.png (alpha gradient)
- frost_overlay.png
- shadow_overlay.png

Entities — characters & creatures (entities/)
- player_idle.png (sprite sheet)
- npc_villager_01.png
- hostile_npc_01.png
- ghost_spirit_01.png (for otherworldly flora/fauna)

Crops & plants (entities/crops/)
- crop1.png, crop2.png, crop3.png, crop4.png (per-crop stage textures; multiple crops each in their own folder)
- bush_berry_01.png
- tree_apple_01.png (additional sizes and LOD variants)

Items (items/)
- seed_wheat.png
- crop_wheat.png
- apple_01.png
- pipe_basic.png
- pump.png
- battery.png
- solar_panel.png
- generator_biomass.png
- tool_wrench.png
- tool_trowel.png

UI (ui/)
- inventory_slot.png (64x64 base for high-dpi, scaled down for normal)
- icon_seed.png (32x32)
- icon_crop.png
- hud_bar.png
- dialog_box.png

Particles & VFX (particles/)
- water_splash_01.png
- steam_small.png
- spark_01.png

Overlays & debug (overlays/)
- water_flow_arrow.png (directional arrow for edges)
- power_heatmap_gradient.png

Scene & background art
- sky_gradient_01.png (wide, matches screen aspect)
- parallax_layer_1.png, parallax_layer_2.png

Audio (brief list — not textures)
- ambient_field_01.ogg
- mechanical_hum_01.ogg
- footstep_grass_01.ogg
- dialog_chime_01.ogg

Naming conventions and metadata
- Each texture should have a small JSON metadata file (same name .json) describing pivot/origin, tile collision shape, and optional collision offsets.
- Example: `entities/player_idle.png` accompanied by `entities/player_idle.json` with fields: frameSize, origin, animations mapping.

Missing details & TODO
- Provide artist‑friendly guidelines (palette, pixel density, shadowing rules).
- Create placeholder art set for prototyping.

If you want, I can create a starter assets archive with placeholder PNGs and metadata JSON files in `assets/`.
