Quests & Narrative — Feature Design and Technical Plan

Overview

Design for personal and systemic quests, relationship events, and memory fragments.

Quest model
- Quest definition JSON: id, title, type, conditions (system state or inventory items), rewards, chained quests.
- Persistent state: track progress, timestamps, choices made.

Narrative events
- Triggers: system events (e.g., "WaterNetStable"), player actions (deliver item), or location visits.
- Character scenes: cutscenes implemented as dialog sequences + camera control + scripted entity actions.

Testing
- Integration tests for event triggers and quest completion conditions.
