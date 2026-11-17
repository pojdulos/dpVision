# GUI Coordination Rules

This document defines which mechanism should be used for which kind of host/UI
coordination.

It exists to avoid mixing three different problems:

- `core/api/plugin -> gui` bridging
- `workspace/object mutation -> gui reaction`
- local `gui -> gui` orchestration

These problems should not be solved by one universal helper.

## Short Rule

Ask this in order:

1. Is this a workspace or object mutation?
   - Use `CWorkspace` events.
2. Is this a call coming from `core`, legacy API, or a plugin-facing adapter?
   - Use `AppStateManager` only if the caller should not know concrete GUI.
3. Is this only local coordination between known GUI components?
   - Call GUI objects directly.

## Mechanism 1: Workspace Events

Use `CWorkspace` notifications when the source action changes:

- workspace membership
- parent/child structure
- current object activation
- object state that affects how existing workspace content is presented

Examples:

- add/remove object
- reparent object
- duplicate object into workspace
- direct visibility change on an object
- direct transform/geometry/label/lock change on an object

Preferred notifications:

- `notifyObjectStateChanged(id)`
  - use when one object's internal state changed
  - the object stays in the same place in the tree

- `notifyStructureChanged()`
  - use when the tree topology changed
  - or when a broad workspace refresh is the safest current behavior

Important:

- prefer semantic workspace/object operations that emit events automatically
- use direct notify calls only as a transition path for legacy code that still
  mutates objects directly

## Mechanism 2: AppStateManager

Use `AppStateManager` as a bridge only when the caller is outside local GUI and
should not know `CMainWindow`, dock widgets, or other concrete GUI classes.

Typical valid callers:

- `src/core/*`
- legacy `AP::` / `UI::`
- plugin-facing adapters in `src/api/adapters/*`

Typical valid actions:

- `adjustForCurrentFile(...)`
- host-driven `updateAllViews(...)`
- host-driven `updateProperties()`
- host-driven `changeMenuAfterSelect()`

`AppStateManager` is appropriate when crossing the boundary into GUI, not as a
general-purpose replacement for local method calls inside GUI code.

## Mechanism 3: Direct GUI Orchestration

Use direct `gui -> gui` calls when:

- the caller already owns or clearly knows the GUI component
- the action is local GUI orchestration
- no isolation boundary is being crossed

Examples:

- `CMainWindow` updating its own dock widgets
- a dock asking its owning window for a local refresh
- viewer-specific presentation logic
- camera/view operations that do not represent workspace mutations

This is usually better than routing the call through `AppStateManager`.

## Practical Classification

### Keep on Workspace Events

- add/remove/activate workspace operations
- object-state mutations that affect workspace presentation
- legacy direct tree rewrites that need one final notify call

### Keep on AppStateManager

- calls originating in `core`
- calls originating in plugin/legacy API adapters
- host-side reactions where the caller should not depend on GUI types

### Prefer Direct GUI Calls

- `MainWindow -> dockProperties`
- `MainWindow -> updateAllViews()`
- local menu/viewer/dock coordination
- pure GUI presentation work with no workspace mutation semantics

## Current Migration Policy

For now:

- workspace/object operations should move toward `CWorkspace` events
- local GUI orchestration should become more direct, not more abstract
- `AppStateManager` should remain only where it is genuinely a boundary bridge

This keeps the architecture simple:

- domain changes are announced by workspace
- GUI-local coordination stays GUI-local
- external callers still have a host bridge
