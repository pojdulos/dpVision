# API Refactor Roadmap

This document turns the target architecture into a practical execution plan.

Use it together with:
- `API_ARCHITECTURE.md` for principles
- `GUI_COORDINATION_RULES.md` for deciding between workspace events,
  `AppStateManager`, and direct GUI orchestration
- `API_INTERFACE_INVENTORY.md` for current classification
- `LEGACY_API_MAP.md` for compatibility mapping
- `CAMERA_VIEW_MODEL_NOTES.md` for the deferred future model where cameras may
  exist independently of concrete viewers

## Primary Outcome

After the refactor:
- `core` builds independently of `api` and `gui`
- `gui` builds on `core` without depending on plugin-facing contracts
- plugins use `api` as the supported host boundary
- `AP::` and `UI::` exist only as compatibility wrappers
- privileged GUI access is explicit and separate

Primary migration rule:
- no refactor step should change bundled or external plugin behavior unless the
  compatibility break is explicit and intentional
- if preserving behavior requires adding host-side support code in `core` or
  `gui`, that is preferred over changing plugin-visible semantics

## Work Sequence

The order matters. Each phase prepares the next one.

## Phase 0: Stabilize Direction

Goal:
- freeze architectural drift before deeper changes start

Tasks:
- keep `API_ARCHITECTURE.md` as the source of truth
- classify new API changes against the inventory before merging
- avoid adding new features to `AP::` or `UI::` except compatibility glue

Exit criteria:
- no new `core -> api`
- no new `gui -> api`
- no new default API methods exposing raw GUI types without an explicit decision

## Phase 1: Remove `core -> api`

Goal:
- make `core` independent again

Tasks:
- find every include of `src/api/adapters/*` from `src/core`
- replace adapter usage with direct `core` logic or lower-level helpers
- remove local `appApi()` helper singletons from `core`

Expected hotspots:
- workspace access
- file loading
- update/status convenience calls

Exit criteria:
- no source file in `src/core` includes `src/api/*`
- `core` can be reasoned about without plugin-facing abstractions

Risk notes:
- some functionality currently routed through `AppStateManager` or host helpers
  may need a local replacement or relocation

## Phase 2: Separate Default API From Privileged GUI API

Goal:
- make the plugin boundary explicit

Tasks:
- define which interfaces belong to the default API root
- define which interfaces belong to the privileged GUI root
- move or split mixed interfaces:
  - `IPluginPanelAPI`
  - `ICameraAPI`
  - `IProgressAPI`
  - dock-related interfaces

Recommended target split:
- default plugin host
- privileged GUI host

Transition note:
- `IPluginHostAPI` is the default host entry point introduced for ordinary plugins
- `IPluginGuiAPI` is the explicit privileged entry point for GUI-aware plugins
- `ICameraControlAPI` and `IProgressControlAPI` hold the safe parts of the
  former mixed camera/progress contracts
- existing `PluginUIAPIAdapter` and `PluginGuiAPIAdapter` remain temporarily as
  compatibility facades during migration

Exit criteria:
- raw GUI access is not reachable through the default plugin API by accident
- privileged GUI access is still available, but through explicit types
- at least one in-tree plugin demonstrates explicit opt-in to privileged GUI
  access
- dock-related APIs are reachable through the privileged GUI root instead of
  only through `IUIAPI`

## Phase 3: Replace `IUIAPI` Composition

Goal:
- remove the mixed null-object aggregate

Tasks:
- deprecate `IUIAPI` as the main composition root
- replace `UIAPIAdapter` with clearer service composition
- update plugin-facing adapter composition to use the new roots
- keep `IUIAPI` available only as a transitional wrapper until in-tree plugins
  stop depending on it
- stop using `UIAPIAdapter` as hidden backing storage for default-safe services
- for mixed domain/view cases such as histogram, split the core model from the GUI
  dock and connect them through a host-side listener registered by `gui`
  inside transitional plugin facades
- move adapter implementation logic off `UI::`/`AP::` in small slices by
  extracting shared host-side helpers where behavior must stay exact

Suggested target:
- `IPluginHostAPI` for default plugins
- `IPluginGuiAPI` for explicit GUI-aware plugins

The exact names are not important. The split is.

Exit criteria:
- there is no need for null-object compatibility adapters in the main
  composition model
- service composition reflects real capabilities

## Phase 4: Clean Default Contracts

Goal:
- make the default API less coupled to Qt and legacy shapes

Tasks:
- review every default-facing interface for Qt types
- replace `QString` with standard strings where practical
- move raw widget/viewer access to privileged contracts
- keep rich `core` object access where plugins need it

Important constraint:
- do not force opaque handles for everything
- domain-rich access to `core` objects is allowed
- when a `core` type still carries a Qt-backed data dependency for plugin
  compatibility, keep that dependency temporarily and extract only GUI/view
  state first

Exit criteria:
- default plugin contracts expose `core` types deliberately
- default plugin contracts do not expose GUI types accidentally

## Phase 5: Invert Legacy Implementation

Goal:
- make `AP::` and `UI::` pure compatibility layers

Tasks:
- reimplement legacy entry points in terms of the real API
- stop using legacy namespaces as a foundation for new adapters
- reduce duplicated logic

Exit criteria:
- `AP::` and `UI::` are thin wrappers
- new work lands in the real API, not in legacy shims

Current implementation guidance for this phase:
- prefer extracting host-side helpers in `gui` for legacy-exact behavior
  (`PluginPanelHostAccess`, `WorkspaceDockHostAccess`, `ImageViewerHost`)
  instead of re-implementing GUI workflow in `UI.cpp`
- prefer extracting shared non-GUI legacy runtime and orchestration helpers
  into `core` when they are used by both `AP::` and `UI::`
  (`LegacyAppRuntime`, object-transfer services, manager-level formatting)
- treat the remaining raw-instance getters in `UI::` as explicit compatibility
  escape hatches, not as a model for new API work
- keep default-vs-privileged boundary checks enforced in code where practical
  (for example compile-time checks in `BoundaryContractChecks.cpp`), not only
  in documentation
- keep legacy GUI refresh semantics moving toward workspace events for
  workspace/object mutations, not toward more `AppStateManager` or direct GUI
  patchwork

## Phase 5a: Stabilize Host Refresh Semantics

Goal:
- make workspace/object mutations refresh GUI through one host-side mechanism

Tasks:
- extend `CWorkspace` eventing where needed
- route workspace/object mutation consequences through workspace events
- keep `AppStateManager` only for `core/api/plugin -> gui` bridging
- simplify local `gui -> gui` orchestration instead of wrapping it in new
  global helpers
- keep transitional legacy wrappers in `AP::WORKSPACE` for direct
  `notifyObjectStateChanged(...)` and `notifyStructureChanged()`

Exit criteria:
- new workspace/object mutation code does not call GUI refresh directly
- legacy direct object mutations can notify workspace without touching GUI
- `AppStateManager` is no longer treated as the default answer for every
  refresh path

## Phase 6: Plugin Migration Pass

Goal:
- move in-tree plugins toward the supported API paths

Tasks:
- migrate example and bundled plugins away from direct legacy usage
- migrate bundled plugins away from transitional facades when the canonical
  host adapters already cover the same behavior
- default plugins should use the default plugin API
- GUI-heavy plugins should opt into the privileged API explicitly

Exit criteria:
- in-tree plugin examples demonstrate the intended architecture
- plugin examples no longer teach legacy usage as the preferred path

## Recommended Initial File Targets

Start here in order:

1. `src/core/Workspace.cpp`
2. `src/core/WorkspaceFile.cpp`
3. other `src/core/*.cpp` files that include `AppAPIAdapter`
4. `src/api/interfaces/IUIAPI.h`
5. `src/api/adapters/UIAPIAdapter.h`
6. `src/api/interfaces/IPluginPanelAPI.h`
7. `src/api/interfaces/ICameraAPI.h`
8. `src/api/interfaces/IProgressAPI.h`
9. `src/api/adapters/PluginUIAPIAdapter.h`
10. `src/api/adapters/PluginGuiAPIAdapter.h`
11. `src/api/AP.*` and `src/api/UI.*`

## Suggested Pull Request Shape

Keep the refactor in small, reviewable slices.

Suggested PR sequence:

1. Remove `core -> api` includes and helper usage
2. Introduce explicit default vs privileged API composition
3. Split mixed interfaces
4. Rewrite legacy wrappers over the new implementation
5. Migrate in-tree example plugins

Each PR should ideally preserve buildability and move only one boundary at a
time.

If a step cannot preserve plugin behavior and architectural cleanliness at the
same time, keep behavior stable first and add the missing support layer.

## Completion Checklist By Phase

### Phase 1 complete when

- `rg "#include \"../api/" src/core` returns nothing relevant
- `src/core` no longer constructs `AppAPIAdapter`

### Phase 2 complete when

- default plugin entry points do not expose `QWidget*`, `QPushButton*`,
  `GLViewer*`, or similar GUI types
- privileged GUI entry points are explicit

### Phase 3 complete when

- `IUIAPI` is removed, deprecated, or reduced to a transitional wrapper only
- null adapters are no longer needed for normal composition

### Phase 4 complete when

- default interfaces are reviewed one by one
- Qt leakage is documented and either removed or intentionally accepted

### Phase 5 complete when

- new implementation logic is no longer anchored in `AP::`/`UI::`
- legacy namespaces delegate downward
- `AP::` replacement work follows `AP_ISOLATION_PLAN.md` instead of ad-hoc
  wrapper rewrites

## What Not To Do

Do not:
- redesign every plugin contract at once
- replace all domain access with opaque handles
- break compatibility before the real API replacement exists
- push GUI internals back into the default plugin path for convenience

## Change Review Questions

Before merging a refactor step, ask:

1. Did this reduce an invalid dependency?
2. Did this make the plugin boundary clearer?
3. Did this remove GUI leakage from the default API?
4. Did this preserve plugin access to domain data?
5. Did this move logic away from `AP::`/`UI::` rather than deeper into them?
6. Did this preserve the exact behavior expected by existing plugins?

If not, the step should probably be re-scoped.
