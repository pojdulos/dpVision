# API Architecture Direction

This document defines the target architecture for the plugin-facing API and is
meant to be the reference point for further refactoring in `src/api`.

It complements `LEGACY_API_MAP.md`:
- `LEGACY_API_MAP.md` explains how old `AP::` and `UI::` names map to the new
  world during migration.
- this document defines what the new world should look like.

Working documents related to this one:
- `API_INTERFACE_INVENTORY.md` classifies current interfaces and adapters
- `API_REFACTOR_ROADMAP.md` defines the execution order for the refactor
- `API_REFACTOR_CHECKLIST.md` is the operational checklist used during changes

## Goals

The API layer exists to expose application capabilities to plugins while
preserving a clean dependency direction between core logic, GUI, and
compatibility code.

Primary migration constraint:
- bundled and external plugins must keep behaving exactly as before unless a
  change is explicitly accepted as a compatibility break
- behavior compatibility takes priority over architectural purity during the
  migration
- adding helper code in `core` or `gui` is allowed when it is needed to
  preserve old plugin semantics while moving dependencies in the right
  direction

The intended high-level structure is:

1. `core` knows nothing about `gui`
2. `core` knows nothing about `api`
3. `gui` knows `core`
4. `gui` does not depend on `api`
5. `api` exposes host capabilities to plugins
6. `AP::` and `UI::` remain only as compatibility shims over the real API
7. access to raw GUI internals is possible, but only through an explicit,
   privileged plugin path

## Dependency Rules

The target dependency graph is:

`core <- gui`

`core <- api`

`core <- api_gui_privileged -> gui`

`api <- legacy_api`

`api_gui_privileged <- legacy_api`

`plugins -> api`

`plugins -> api_gui_privileged` only when the plugin author explicitly chooses
to depend on privileged GUI access.

The following dependencies are not allowed in the target state:

- `core -> api`
- `core -> gui`
- `gui -> api`
- `plugins -> AP::/UI::` for new code

## Architectural Roles

### Core

`core` owns:
- domain objects
- data structures
- algorithms
- persistence logic
- rendering-independent application state

`core` must not:
- include plugin API adapters
- know about plugin-facing facades
- know about Qt widgets, dock panels, viewer windows, or similar GUI concepts

`core` may still expose rich object models. Plugins are allowed to work with
real application data. Isolation is about dependency ownership and host
boundaries, not about artificially hiding all domain types.

### GUI

`gui` owns:
- windows
- dock widgets
- plugin panels
- progress widgets
- viewer widgets
- UI event handling

`gui` should operate directly on `core` and on GUI-local adapters/helpers.

`gui` should not depend on the plugin-facing API contracts just to call into the
application itself. If `gui` needs a helper abstraction, that abstraction should
live in `gui` or in a lower-level host/service layer, not in `src/api`.

### API

`api` owns:
- stable plugin-facing contracts
- default host implementations backed by `core`
- optional service composition for plugins
- migration path away from `AP::` and `UI::`

The API should describe capabilities, not expose random singleton entry points.
Its job is to isolate plugins from host wiring decisions.

### Legacy API

`AP::` and `UI::` stay available for compatibility, but their role is narrow:
- preserve source compatibility for existing plugins
- forward to the real API implementation
- gradually shrink

They are not the foundation of the new architecture.

## Two Plugin Access Levels

The target model has two explicit plugin access levels.

### 1. Default plugin API

This is the normal path and should cover the majority of plugin use cases.

Properties:
- safe by default
- independent from raw GUI widget access
- focused on operations and services
- may expose rich domain objects from `core` where this is useful and stable

Examples:
- workspace/model/object operations
- file dialogs
- status/progress reporting
- message routing
- settings
- import/export actions

Current transition entry point:
- `IPluginHostAPI`

### 2. Privileged GUI API

This path is opt-in and intentionally more dangerous.

Properties:
- exposes raw GUI capabilities
- may expose Qt widgets, viewers, docks, or direct GUI hooks
- should require deliberate use by the plugin author
- should be clearly separated in naming, headers, and implementation

Examples:
- access to the current `GLViewer`
- direct `QWidget*` handles
- plugin panel host internals
- camera/viewer internals

Current transition entry point:
- `IPluginGuiAPI`

The distinction must be obvious in code. A plugin author should not gain raw GUI
access by accident through the default API.

Current split for camera/progress responsibilities:
- safe camera operations belong to `ICameraControlAPI`
- safe progress reporting belongs to `IProgressControlAPI`
- raw viewer/camera/progress widget access belongs to `IGuiInternalsAPI`

Current classification for plugin panels:
- `IPluginPanelAPI` is treated as privileged GUI API
- `IUIAPI::pluginPanel()` remains only as a transitional compatibility path

Current classification for dock access:
- `IDockWorkspaceAPI` and `IDockHistogramAPI` are treated as privileged GUI
  capabilities
- `IUIAPI` may still expose them temporarily as a compatibility aggregate

## What Isolation Means Here

Isolation does not mean plugins must be limited to opaque integer handles for
everything.

For this project, plugins still need full access to application data. That is a
valid requirement and should be preserved.

Therefore:
- exposing `core` domain types to plugins can be acceptable
- exposing `gui` implementation types through the default API is not acceptable
- exposing legacy singleton wiring through the default API is not acceptable

This means the default API may still return or accept types like:
- `std::shared_ptr<CModel3D>`
- `std::shared_ptr<CBaseObject>`
- domain enums and geometry types from `core`

But it should not, by default, expose:
- `QWidget*`
- `QPushButton*`
- `GLViewer*`
- `ProgressIndicator*`
- direct dock widget classes

Those belong in the privileged GUI API only.

## API Design Principles

### Prefer capability-oriented services

The API should be organized around clear responsibilities, for example:
- app/workspace/object/model services
- messaging/status/progress services
- file and settings services
- optional GUI services

Avoid giant grab-bag facades that mix unrelated concerns.

### Prefer semantic operations over singleton forwarding

Bad direction:
- thin wrappers that merely rename `AP::` or `UI::`
- adapters that just call a singleton and return the same internal object

Preferred direction:
- define the operation the plugin needs
- implement that operation against `core` and, where explicitly needed,
  against `gui`

### Keep the default API GUI-agnostic

The default API should use standard C++ and core-domain types where possible:
- `std::string` instead of `QString` in default-facing contracts
- standard containers
- domain value types from `core`

Qt types are acceptable in:
- compatibility shims
- privileged GUI APIs
- host-side implementation details

Temporary compatibility exception:
- selected Qt-backed data types may remain in `core` when removing them would
  likely break existing plugins or force large-scale rewrites
- `CImage` inheriting from `QImage` is currently treated as such an accepted
  compatibility constraint
- in such cases, the preferred direction is to move view-only state and viewer
  orchestration out of `core` into `gui`, not to force an immediate data-model
  rewrite

### Keep legacy compatibility outside the core architecture

Compatibility overloads for `char*`, `std::string`, `std::wstring`, and old
namespace shapes are acceptable in `AP::`/`UI::` and migration helpers.

They should not dictate the shape of the real plugin-facing architecture.

The same rule applies to mixed aggregate facades such as `IUIAPI`: they may
remain temporarily, but should not be treated as the target composition model.

## Current Problems To Eliminate

The current codebase contains patterns that are useful during migration but are
not part of the target architecture:

1. `core` includes `api` adapters
2. public interfaces expose GUI classes in the default path
3. many adapters are only 1:1 forwarders to `AP::`/`UI::`
4. some plugin-facing facades bypass their own interface layer and call
   application singletons directly
5. `AP::`/`UI::` are still effectively part of the implementation base rather
   than compatibility wrappers

Each of these should be treated as migration debt.

## Target Module Split

The long-term split should be close to the following:

- `src/core`
  - domain logic only

- `src/gui`
  - GUI implementation over `core`

- `src/api/interfaces`
  - default plugin-facing contracts

- `src/api/adapters`
  - host implementations of default contracts, primarily over `core`

- `src/api/gui_interfaces`
  - privileged GUI contracts

- `src/api/gui_adapters`
  - privileged GUI implementations over `gui`

- `src/api/legacy`
  - `AP::`, `UI::`, and migration helpers

Exact folder names may still change, but the role split should remain.

## Migration Strategy

Refactoring should happen incrementally and keep external plugins buildable as
far as practical.

When architecture and compatibility are in tension, compatibility wins for the
current step. The preferred resolution is to add or expose a host-side helper
in `core` or `gui` that preserves the old behavior while allowing the new API
boundary to be introduced safely.

### Phase 1: establish direction

- keep this document current
- stop introducing new dependencies that violate the target graph
- stop adding new functionality directly to `AP::`/`UI::` unless it is strictly
  compatibility-related

### Phase 2: remove `core -> api`

- eliminate includes of plugin-facing adapters from `core`
- replace them with direct `core` logic or lower-level host services
- make `core` stand on its own again

This is the first hard architectural boundary.

### Phase 3: split default API from privileged GUI API

- identify which interfaces are truly GUI-neutral
- move raw widget/viewer access to explicit privileged contracts
- make default plugin adapters depend only on `core` where possible

### Phase 4: shrink Qt usage in default interfaces

- prefer `std::string` in default plugin contracts
- keep Qt-based compatibility overloads in legacy wrappers or privileged APIs
- keep domain-rich access where it helps plugin authors work with real data

### Phase 5: invert legacy implementation

- make `AP::` and `UI::` delegate to the real API
- remove implementation duplication
- keep only compatibility logic in the legacy layer

## Review Checklist For New Changes

When changing `src/api`, review against these questions:

1. Does this introduce a new `core -> api` or `gui -> api` dependency?
2. Is this capability meant for ordinary plugins or only privileged GUI-aware
   plugins?
3. Does the default API expose a GUI type unnecessarily?
4. Is this adapter adding a real abstraction, or only renaming an old call?
5. Could this change be implemented in the real API first, with `AP::`/`UI::`
   delegating afterward?
6. Does this preserve plugin access to domain data without coupling them to GUI
   internals?

If the answer to question 1, 3, or 4 is "yes", the change is likely going in
the wrong direction.

## Non-Goals

The target architecture does not require:
- hiding all core types behind opaque handles
- removing rich domain access from plugins
- eliminating all compatibility layers immediately
- making GUI internals impossible to reach

The goal is controlled access with clear dependency ownership, not artificial
restriction for its own sake.

## Short Version

The intended end state is:

- `core` is independent
- `gui` is built on `core`
- `api` is the plugin boundary
- `AP::` and `UI::` are compatibility wrappers only
- ordinary plugins get a default API without accidental GUI leakage
- advanced plugins can opt into privileged GUI access explicitly
- plugins may still work on real domain objects from `core`

Any refactoring in `src/api` should move the codebase closer to that shape.
