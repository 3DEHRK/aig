# Sound Asset Requests
Format: 16-bit WAV or OGG. Provide clean loop points where loops indicated. Keep average loudness consistent (target -14 LUFS), minimal mastering compression.

## Core UI
| Asset ID | Description | Type | Length | Loop | Notes |
|----------|-------------|------|--------|------|-------|
| ui_click | Generic UI click | SFX | <0.3s | No | Button / toggle |
| ui_toggle_panel | Inventory open/close | SFX | <0.5s | No | Soft whoosh + click layer |
| ui_error | Invalid action | SFX | <0.4s | No | Gentle blip, no harsh buzz |

## Player & Combat
| Asset ID | Description | Type | Length | Loop | Notes |
| player_step_grass | Footstep on grass | SFX | <0.25s | No | Provide 4 variants for variety |
| player_hit | Player damage feedback | SFX | <0.4s | No | Subtle thud + airy hiss |
| projectile_fire | Firing projectile | SFX | <0.4s | No | Light arcane zap |
| projectile_hit | Impact on enemy | SFX | <0.4s | No | Soft pop + spark |
| hostile_grunt_attack | Grunt attack vocalization | SFX | <0.6s | No | 3 variants |
| hostile_tank_step | Heavy tank step | SFX | <0.5s | No | Low thump, 2 variants |
| hostile_death | Generic hostile death | SFX | <0.8s | No | Crunch + dissipate tail |

## Farming & Environment
| Asset ID | Description | Type | Length | Loop | Notes |
| water_splash_small | Watering can splash | SFX | <0.6s | No | Light droplets |
| fertilize_apply | Fertilizer sprinkle | SFX | <0.7s | No | Granular trickle |
| harvest_crop | Harvest action | SFX | <0.6s | No | Plant rustle + soft pluck |
| pickup_item | Item pickup sparkle | SFX | <0.5s | No | Bell + faint chime |

## Rails & Logistics (Future)
| Asset ID | Description | Type | Length | Loop | Notes |
| cart_move_loop | Moving cart rumble | SFX | 3-5s | Seamless loop | Light metal rattle |
| cart_brake | Cart slowing | SFX | <0.8s | No | Brief squeak |
| rail_place | Rail placement | SFX | <0.5s | No | Metallic click |
| rail_remove | Rail removal | SFX | <0.5s | No | Reverse click |

## Ritual & Realm (Future)
| Asset ID | Description | Type | Length | Loop | Notes |
| altar_activate | Altar activation | SFX | <1.2s | No | Rising harmonic swell |
| portal_ambient_loop | Portal idle ambience | Ambience | 10-20s | Loop | Ethereal low shimmer |
| realm_enter | Realm entry transition | SFX | <1.0s | No | Reverse inhale + chime |

## Ambient Loops (Layered)
| Asset ID | Description | Type | Length | Loop | Notes |
| ambience_daylight | Daytime outdoor | Ambience | 30s | Loop | Light breeze + subtle birds |
| ambience_night | Night outdoor | Ambience | 30s | Loop | Crickets + soft wind |
| ambience_cave | Subterranean rumble | Ambience | 30s | Loop | Low drone + dripping |

## UI / Feedback Extras
| Asset ID | Description | Type | Length | Loop | Notes |
| codex_open | Codex panel open | SFX | <0.5s | No | Soft paper flip + chime |
| codex_close | Codex panel close | SFX | <0.5s | No | Reverse paper flip |
| quest_update | Quest stage advance | SFX | <0.7s | No | Ascending tri-tone |

## Power & Systems (Future)
| Asset ID | Description | Type | Length | Loop | Notes |
| generator_loop | Basic generator hum | Ambience | 8-12s | Loop | Smooth medium-low hum |
| power_warning | Low power alert | SFX | <0.7s | No | Soft pulsing beep (avoid alarm harshness) |

## Priorities (Immediate to Future)
1. ui_click, ui_toggle_panel, player_hit, projectile_fire, projectile_hit, harvest_crop, pickup_item
2. hostile_grunt_attack, hostile_death, water_splash_small, fertilize_apply
3. ambience_daylight, ambience_night (base ambience bed)
4. Remaining combat & farming SFX
5. Rails & cart set
6. Ritual & portal set
7. Power & advanced ambience

## Style Guide
- Avoid overly bright high frequencies; keep mellow.
- Short pre-delays (<20ms) for immediacy.
- Reverb subtle; dry/wet < 20% for UI.
- Ensure loops are seamless (zero-cross fade if needed).
