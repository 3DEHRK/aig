Damage System Unification
=========================

Overview
--------
A lightweight damage hook (`Entity::onDamaged(float)`) plus a funnel method (`Entity::applyDamage(float)`) unify side-effects of health reduction (visual feedback, aggro, logging) without duplicating logic in each caller.

Components
----------
- Entity::takeDamage(float): Existing per-entity health decrement logic (may include invulnerability checks etc.).
- Entity::onDamaged(float): New overridable hook for reactions (flash sprite, spawn particles, trigger AI state changes). Default no-op.
- Entity::applyDamage(float): New helper that calls `takeDamage` then `onDamaged` ensuring hook always fires when used.

Current Implementations
-----------------------
- Player: onDamaged placeholder (future: screen flash, sound). Invulnerability handled in takeDamage.
- HostileNPC: onDamaged placeholder (future: aggression escalation, knockback).
- Projectile impacts now call `applyDamage` instead of direct `takeDamage`.

Next Steps
----------
1. Migrate any remaining direct takeDamage external calls to applyDamage (except internal chained logic like load-state reconstruction where raw setting may be intentional).
2. Add visual/audio feedback in overrides.
3. Introduce DamageEvent struct if more metadata (source, type, crit) becomes necessary.
4. Consider tunables for damage flash duration, invulnerability window.

Testing Notes
-------------
Headless tests can validate that calling applyDamage on entities decrements health identically to previous behavior.
