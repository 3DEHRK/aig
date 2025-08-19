# Hostile NPCs & Ecosystem Threats

## Goals
Provide periodic pressure, territorial dynamics, and resource sinks without overwhelming cozy pacing. Hostiles escalate logically with player footprint (infrastructure density, power emissions, realm resonance imbalance).

## Archetype Tiers
1. Forager Vermin (nuisance crop eaters) – low HP, retreat at half health.
2. Stalker Beast – chases player, applies bleed DOT.
3. Corrupted Construct – slow, area suppression aura disrupting power lines.
4. Rift Wraith – phase walker spawning from realm instability.
5. Guardian Echo (boss-class) – required for transcendence ritual catalysts.

## Spawn Logic
Base weight influenced by:
- Biome Threat Level (aggregated from over-farming, pollution, realm tears).
- Infrastructure Heat (power output + rail traffic → noise value).
- Time of Day / Weather (night spawns, storm multipliers).

## AI Layers
1. Perception: Vision cone + vibration sense (detects rails movement).
2. Intent Selection: Finite state (Patrol, Stalk, Attack, Flee, Despawn).
3. Pathing: Greedy steering now; upgrade to flow fields for packs.
4. Combat Execution: Attack wind-up windows telegraphed via color flash & sound swell.

## Combat Stats Schema
```
struct HostileStats {
  float maxHealth;
  float moveSpeed;
  float attackRange;
  float attackCooldown;
  DamageProfile damage; // per-type amounts
  Resistances resist;    // map<DamageType,float>
  float fleeThreshold;   // hp ratio to disengage
}
```

## Damage Types
- Physical (baseline)
- Arc (power network feedback)
- Corrupt (realm instability, bypasses some armor)
- Pure (ritual, effective vs corrupt)

## Escalation
As player automates more (tracked via AutomationScore), threat level schedules mini-raids: group patterns (flankers + bruiser). Telegraph events (ominous hum) giving prep window.

## Loot & Progression
Hostiles drop organic matter (compost input), arc shards (power crafting), corruption residue (ritual neutralizer). Bosses yield unique catalysts unlocking new altar tiers.

## Realm Instability Interaction
Unsealed altar or failed ritual increases Rift Index. Exceed threshold → periodic Wraith spawns until player performs stabilizing rite.

## Status Effects
- Bleed: HP drain over time; stack-limited.
- Shock: Temporarily disables power nodes in small radius on death explosion.
- Corrupt Bloom: Spreads contamination to adjacent soil tiles.

## Balancing Principles
- TTK (time-to-kill) early mobs: 3–5 basic projectile hits.
- No silent one-shots; lethal attacks always foreshadowed.
- Retreat logic prevents cleaning entire biome instantly; respawn timers scale with player expansion.

## Future Extensions
Faction reputation causing negotiated truces, taming mechanics, or converting constructs into defensive turrets after purification.
