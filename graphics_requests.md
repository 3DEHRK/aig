# Graphics Asset Requests
Unified Pixel Base: 32x32 world tile grid. Character/entity sprites align to 32px cell unless noted. All PNGs with transparent background, pixel-art crisp (no filtering), origin centered unless UI icon.

## Legend
- Size column lists width x height in pixels per frame.
- If multiple frames, supply a horizontal strip unless stated (easier simple slicing).
- Palette: moderately low saturation; avoid heavy anti-alias.

## Existing (Reference / To Retain or Replace Later)
- player_idle.png (placeholder) 32x32
- npc_idle.png 32x32
- crop1-4.png 32x32 (staged wheat/herb placeholders)

## Core Characters & NPCs
| Asset ID | Description | Size | Frames | Notes |
|----------|-------------|------|--------|-------|
| player_base | Player idle + 4-dir walk | 32x32 | 4 idle, 8 walk (2 per dir) | Directions: up/down/left/right; separate sheet ok |
| hostile_grunt | Fast melee creature | 32x32 | 4 walk loop | Red tint baseline |
| hostile_tank | Slow, high HP creature | 48x48 | 6 walk loop | Larger silhouette; centered over tile + padding |
| npc_villager_m | Friendly NPC male | 32x32 | 4 idle, 4 walk | Color variant system later |
| npc_villager_f | Friendly NPC female | 32x32 | 4 idle, 4 walk |  |

## Farming
| Asset ID | Description | Size | Frames | Notes |
| crop_wheat | Wheat growth stages | 32x32 | 4 | Stage 0 sprout → 3 ripe; consistent root alignment |
| crop_herb | Herb growth stages | 32x32 | 4 | Color shift + size scale |
| soil_tile | Fertile soil base tile | 32x32 | 1 | Slight texture, neutral brown |
| soil_moist_overlay | Moisture highlight | 32x32 | 1 | Semi-transparent blue overlay |
| soil_lowfert_overlay | Low fertility marker | 32x32 | 1 | Subtle desaturated mask |
| fertilizer_basic | Basic fertilizer item icon | 32x32 | 1 | Bag or vial icon |
| watering_can | Watering tool icon | 32x32 | 1 | Side view |
| seed_wheat_icon | Wheat seed item | 32x32 | 1 | Pouch or single seed cluster |
| seed_herb_icon | Herb seed item | 32x32 | 1 | Different color accent |

## Items / Loot
| Asset ID | Description | Size | Frames | Notes |
| fiber_common | Common fiber drop icon | 24x24 (center in 32) | 1 | Soft plant fiber bundle |
| crystal_rare | Rare crystal drop | 24x24 (center) | 1 | Glow outline optional |
| salve_small | Healing salve consumable | 32x32 | 1 | Corked jar |
| inventory_slot_bg | Inventory slot background | 36x36 | 1 | Slight inset shading |
| icon_save | Save indicator | 16x16 | 1 | Floppy / rune glyph |
| icon_load | Load indicator | 16x16 | 1 | Arrow down into box |

## Combat / Effects
| Asset ID | Description | Size | Frames | Notes |
| projectile_basic | Player projectile | 16x16 | 4 | Rotating or pulsing animation |
| hit_flash | Impact effect | 32x32 | 4 | Short radial burst; additive |
| loot_pickup_fx | Pickup sparkle | 32x32 | 6 | Loop or one-shot |

## Rails & Logistics (Upcoming)
| Asset ID | Description | Size | Frames | Notes |
| rail_straight | Straight rail tile | 32x32 | 1 | Connectors centered |
| rail_curve | Curve piece | 32x32 | 1 | 90° corner |
| rail_t_junction | T junction | 32x32 | 1 | 3-way connectors |
| rail_cross | 4-way junction | 32x32 | 1 |  |
| cart_empty | Basic cart | 32x32 | 4 | 4-dir or wheel anim |
| cart_loaded | Cart with goods | 32x32 | 4 | Overlay variant |

## Ritual / Realm (Future)
| Asset ID | Description | Size | Frames | Notes |
| altar_base | Altar structure Tier 1 | 48x48 | 1 | Centered, subtle glow |
| altar_active_glow | Activation aura | 64x64 | 6 | Loop additive |
| portal_node | Realm portal core | 64x64 | 8 | Pulsing ring |

## Power & Systems (Future)
| Asset ID | Description | Size | Frames | Notes |
| generator_basic | Basic generator | 48x48 | 4 | Light pulse |
| storage_cell | Energy storage | 32x48 | 2 | Charge flicker |
| lamp_basic | Lamp object | 16x32 | 2 | On/off |

## UI / Overlays
| Asset ID | Description | Size | Frames | Notes |
| hud_health_bar_frame | Health bar frame | 220x18 | 1 | 1px border, transparent center |
| minimap_mask | Rounded minimap mask | 200x200 | 1 | Soft vignette alpha |
| dialog_panel | Dialog background panel | 960x200 | 1 | 9-slice candidate if needed |
| codex_panel | Codex UI panel | 260x360 | 1 | Scroll indicator placeholder |

## Hidden Location & Effects
| Asset ID | Description | Size | Frames | Notes |
| hidden_marker | Hidden location marker | 24x24 | 4 | Fading pulse |
| discovery_ping | Discovery radial | 64x64 | 6 | Expanding ring |

## Misc Particles (Future System)
| Asset ID | Description | Size | Frames | Notes |
| particle_leaf | Falling leaf | 16x16 | 4 | Spin sequence |
| particle_spark | Small spark | 8x8 | 4 | Additive |
| particle_water_drop | Water splash droplet | 16x16 | 4 | Splash cycle |

## Style Guide Quick Notes
- Outline thickness: 1px selective (not full black unless needed for contrast).
- Light source: top-left slight (classic pixel RPG convention).
- Avoid sub-pixel gradients; use flat clusters.

## Priority Order (Immediate Needed First)
1. player_base, hostile_grunt, hostile_tank
2. crop_wheat, crop_herb, soil_tile, watering_can, fertilizer_basic, seed icons
3. projectile_basic, hit_flash
4. fiber_common, crystal_rare
5. rail set (straight, curve, junctions) + cart assets
6. altar_base + altar_active_glow
7. Remaining UI panels & effects.
