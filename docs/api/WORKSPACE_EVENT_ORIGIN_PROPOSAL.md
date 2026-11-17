# Workspace Event Origin Proposal

This note captures a deferred design idea for carrying logical operation origin
through workspace event chains.

Context:
- current `CWorkspace` events carry object identity and event kind
- GUI receivers can see the immediate sender of a Qt signal, but not the
  logical initiator of the whole mutation chain
- this makes it harder to distinguish:
  - property-panel initiated edits
  - viewer/mouse initiated edits
  - workspace-tree initiated edits
  - plugin or legacy API initiated edits
  - purely programmatic host-side updates

## Problem Statement

Many feedback loops in the host are structurally correct but too coarse in
their reaction because the final receiver lacks the origin of the operation.

Typical sequence:

1. a property editor changes object state
2. the object or host code emits `notifyObjectStateChanged(id)`
3. GUI listeners refresh in response
4. the originating property editor loses focus, selection, or edit context

The same shape appears in other places:
- viewer interactions
- workspace-tree operations
- plugin-triggered host changes
- scripted/programmatic updates

The issue is not the immediate Qt sender. The useful missing context is the
first logical initiator of the chain.

## Goal

Allow workspace events to carry optional origin metadata so receivers can make
better refresh-policy decisions without replacing the existing event model.

The metadata should help with:
- suppressing self-echo refresh side effects
- choosing between light refresh and full rebuild
- keeping focus/selection/edit session stable where appropriate
- diagnostics and logging of mutation chains

Primary first-priority goal:
- GUI components must be able to detect and suppress reactions to mutation
  events that were initiated by the same GUI component

Primary purpose of that first step:
- prevent self-echo feedback loops in GUI
- avoid losing focus, cursor position, selection, or active edit session
- avoid rebuilding or reapplying GUI state when the originating component
  already knows the change it just made

This is more important than full cross-system provenance tracking.
In other words, the first implementation priority is not "know everything about
who started the whole chain", but "do not let GUI react destructively to its
own returned workspace signals".

It must not become a substitute for:
- correct workspace event semantics
- proper `AppStateManager` usage
- direct local `gui -> gui` orchestration where that is already the right tool

## Proposed Model

Extend workspace event metadata with an optional logical origin record.

Illustrative shape:

```cpp
enum class WorkspaceEventOriginKind {
    Unknown,
    PropertyEditor,
    WorkspaceTree,
    ViewerInteraction,
    Plugin,
    LegacyApi,
    Programmatic,
    FileLoad
};

struct WorkspaceEventOrigin {
    WorkspaceEventOriginKind kind = WorkspaceEventOriginKind::Unknown;
    std::uint64_t correlationId = 0;
    int pluginId = 0;
    const char* tag = nullptr;
};

struct WorkspaceEvent {
    WorkspaceEventType type;
    int objectId = NO_CURRENT_MODEL;
    CBaseObject::Type objectType = CBaseObject::Type::GENERIC;
    WorkspaceEventOrigin origin;
};
```

The exact field set is not important yet. The important part is:
- stable origin kind
- optional correlation for a whole chain
- optional small extra detail when useful

## Minimal Scope

The first version should stay intentionally small.

Recommended first step:
- add only the minimum origin marker needed for GUI self-echo suppression
- make it possible for a GUI component to mark workspace mutations as
  originating from itself
- let receiving GUI code decide between:
  - ignore self-echo
  - preserve current edit session and do only a light refresh
- optionally add `WorkspaceEventOriginKind`
- optionally add `correlationId`
- default everything else to `Unknown`

Avoid in the first slice:
- deep provenance stacks
- arbitrary string payloads everywhere
- event-specific metadata explosion
- broad refactors of all call sites at once

## Recommended Origin Kinds

Suggested initial set:
- `Unknown`
- `PropertyEditor`
- `WorkspaceTree`
- `ViewerInteraction`
- `Plugin`
- `LegacyApi`
- `Programmatic`
- `FileLoad`

Optional later split if needed:
- `RemoteCommand`
- `ImportPipeline`
- `UndoRedo`

## Example Uses

### Properties Dock

If `origin.kind == PropertyEditor` and the dock is updating the same object:
- prefer a light in-place refresh
- preserve active editor focus
- avoid rebuilding the widget tree unless structure actually changed

### Viewer-Originated Changes

If `origin.kind == ViewerInteraction`:
- properties dock may refresh values
- workspace tree may update presentation state
- local viewer updates still stay direct where appropriate

### Plugin-Originated Changes

If `origin.kind == Plugin` or `LegacyApi`:
- preserve current GUI bridge behavior
- optionally log or trace the chain for diagnostics

### Programmatic Updates

If `origin.kind == Programmatic`:
- allow conservative refresh policy
- avoid pretending the update came from a user-driven editor session

## Integration Strategy

Keep this incremental.

Recommended order:

1. Extend `WorkspaceEvent` with optional origin metadata.
2. Add overloads or helper context for:
   - `notifyObjectStateChanged(...)`
   - `notifyStructureChanged(...)`
   - `notifyObjectAdded(...)`
   - `notifyObjectRemoved(...)`
3. Update a very small number of callers first:
   - properties dock / property widgets
   - workspace tree actions
   - selected viewer interaction paths
4. Teach only the receivers that materially benefit:
   - properties dock first
   - possibly workspace tree second
5. Leave untouched call sites on `Unknown` until there is a real need.

## Suggested API Shapes

Two viable approaches:

### Option A: explicit overloads

```cpp
void notifyObjectStateChanged(int id);
void notifyObjectStateChanged(int id, WorkspaceEventOrigin origin);
```

Pros:
- obvious call sites
- easy migration

Cons:
- repetitive overload growth

### Option B: scoped event context

```cpp
class WorkspaceEventOriginScope {
public:
    explicit WorkspaceEventOriginScope(WorkspaceEventOrigin origin);
    ~WorkspaceEventOriginScope();
};
```

Then plain notify methods would attach the current ambient origin if one exists.

Pros:
- less boilerplate in mutation-heavy UI paths
- correlation across multi-step chains is easier

Cons:
- more hidden behavior
- harder to reason about if overused

For this codebase, Option A is probably the safer first step.

## Interaction With Existing Coordination Rules

This proposal does not change the current rule set:
- workspace/object mutation still uses `CWorkspace` events
- `AppStateManager` remains only a boundary bridge
- local `gui -> gui` orchestration remains direct

Origin metadata only refines how receivers react to a valid event.

It should not be used to justify:
- routing local GUI work through workspace events unnecessarily
- keeping invalid global refresh paths alive
- skipping needed updates because the origin "looks local"

## Risks

Main risks:
- overfitting refresh policy to origin instead of event semantics
- introducing too many origin values too early
- using origin to encode business logic rather than UI reaction policy
- partial rollout causing confusion if `Unknown` is not handled well

Mitigations:
- keep `Unknown` as the default-safe path
- use origin only for refresh policy and diagnostics at first
- migrate only a few high-value callers first

## First Candidate Use Case

The best first target is the properties dock.

Why:
- it already suffers from self-echo refresh issues
- the user-facing regression is obvious
- the policy difference is clear:
  property-editor initiated changes should preserve edit session context better
  than generic external updates

First-priority success condition:
- a GUI component that initiated a workspace/object-state change does not
  destructively react to the same change when it comes back through workspace
  events

Concrete examples:
- properties dock keeps focus and edit state after its own value change
- workspace tree does not over-refresh after its own local toggle/relabel action
- viewer interaction paths do not fight their own returned update signals

If this works well there, the same mechanism can later be reused for:
- viewer interactions
- workspace tree operations
- plugin-triggered mutations

## Deferred Decision

This document is intentionally only a design placeholder.

No implementation commitment yet on:
- exact enum names
- exact metadata fields
- whether correlation ids are required from day one
- whether `StructureChanged` should always carry origin

When revisiting this topic, start by validating the properties-dock use case and
choose the smallest implementation slice that solves that problem without
restructuring the whole event system.
