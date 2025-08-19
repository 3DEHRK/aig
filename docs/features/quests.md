# Quests & Progression Directives

## Philosophy
Avoid linear scripted chains; provide contextual nudges (Directives) arising from player world state data (inventory milestones, biome metrics, discoveries). Traditional quests exist for landmark achievements/boss gates.

## Elements
- Directive: Lightweight goal suggestion with dynamic text.
- Quest: Multi-step structured objective (may unlock systems).
- Codex Entry: Lore snippet unlocked by discovery/ritual.

## Directive Generation
Runs periodic evaluator scanning conditions:
- If player has ≥ X raw dongles but no altar built → suggest locating resonance site.
- Soil fertility average < threshold → suggest crop rotation / compost crafting.
- High contamination + low water purity → suggest building filtration pump.

Template system with tokens:
`"Your fields show signs of fatigue (avg fertility {fert_avg}%). Try rotating crops."`

## Quest Structure
```
struct QuestStep { std::string id; Condition cond; Reward reward; }; 
struct Quest { std::string id; std::vector<QuestStep> steps; size_t current; State state; };
```

States: Inactive, Active, Completed, Failed (rare timed ritual cases).

## Conditions
- ItemPossessed(id,count)
- EntityConstructed(type,count)
- LocationDiscovered(id)
- RitualCompleted(tag)
- HostileDefeated(type,count)

## Rewards
- Items, UnlockFlags (enable crafting recipes), StatBuffs, Codex Entries.

## UI
Quest Journal Panel:
- Active tab: steps with progress bars.
- Lore tab: codex entries with filters (Realm, Ecology, Artifacts).
- Directive banner (compact) appears until dismissed or satisfied.

## Event Hooks
- OnItemAdded → updates ItemPossessed conditions.
- OnEntityBuilt → updates infrastructure quests.
- OnRitualComplete → triggers chain expansions.

## Persistence
Serialize quests with current step index; directive queue state; unlocked codex IDs.

## Balancing & Flow
Ensure first structured quest line unlocks irrigation + power prototypes within ~25 real minutes (average pace). Avoid deadlocks by offering alternate directive when key resource absent.

## Extensibility
Data-driven quest JSON: steps list referencing condition factory tokens. Allows rapid iteration without recompiling.
