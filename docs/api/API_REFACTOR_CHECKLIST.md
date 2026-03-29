# API Refactor Checklist

This file is a practical working checklist for implementing the roadmap.

Mark items off as work progresses. If a task is intentionally deferred, note why
in the related commit or PR description.

## Current Status

- [x] Phase 1: remove `core -> api`
      direct source-level `core -> api` includes used by normal compilation
      were removed; only commented historical include traces remain
- [~] Phase 2: split default API from privileged GUI API
      default and privileged roots exist, compile-time boundary checks are in
      place, dock/camera/progress/plugin-panel GUI access is explicit, and
      `dpVisionGui` no longer links `dpVision::LegacyApi`; remaining work is
      mostly privileged legacy-surface cleanup in `UI::` and API-side GUI
      separation
- [~] Phase 5: invert legacy implementation
      `AP.cpp` is now wrapper-oriented and `UI.cpp` has been partially thinned;
      some legacy helper logic still remains in `UI.cpp`
- [~] Phase 5a: stabilize host refresh semantics
      workspace event infrastructure now includes `ObjectStateChanged` and
      `StructureChanged`, and legacy `AP::WORKSPACE` wrappers exist; migration
      of callers is still in progress

## Migration Rule

- [ ] Preserve exact plugin behavior for each refactor step
- [ ] If behavior would change, add host-side support in `core` or `gui`
      instead of changing plugin-visible semantics

## Boundary Cleanup

- [x] Remove all includes of `src/api/*` from `src/core/*`
- [x] Remove local `appApi()` helper singletons from `src/core/*`
- [x] Verify `src/gui/*` does not depend on `src/api/interfaces/*` just to call
      host functionality
- [x] Confirm no new code in `core` is built on `AP::` or `UI::`
- [~] Confirm no new code in `gui` is built on `AP::` or `UI::`
      `gui` now uses host-access helpers for the reviewed paths; keep checking
      this rule on new changes
- [x] Keep `dpVisionGui` independent from `dpVision::LegacyApi`

## Interface Split

- [ ] Classify every interface as default, privileged, transitional, or legacy
- [x] Split camera access into safe operations vs raw GUI internals
- [x] Split progress access into progress reporting vs widget internals
- [x] Move or split `IPluginPanelAPI` out of the default path
- [x] Classify `IPluginPanelAPI` as privileged-only while keeping
      `UI::PLUGINPANEL::*` as the public plugin facade
- [x] Confirm dock-related APIs are privileged-only or host-internal

## Contract Cleanup

- [ ] Review default interfaces for `QString`
- [x] Review default interfaces for `QWidget*`
- [x] Review default interfaces for `QPushButton*`
- [x] Review default interfaces for `GLViewer*`
- [ ] Review default interfaces for `ProgressIndicator*`
- [ ] Review default interfaces for `void*` GUI escape hatches
- [ ] Keep `core` domain types only where they are intentionally part of the
      supported plugin surface

## Adapter Cleanup

- [ ] Identify adapters that only forward to `AP::` or `UI::`
- [~] Rebuild default adapters directly over `core` where possible
      camera/progress adapters now use shared host-side helpers instead of
      calling `UI::` directly, selected legacy runtime/status logic was moved
      below `UI.cpp`; legacy image insertion now goes through
      `WorkspaceAPIAdapter::addModel(...)` with viewer policy stored on
      `CImage`; `PluginPanelAPIAdapter` now goes through
      `PluginPanelManager` instead of including GUI host access directly, and
      `GuiCameraAPIAdapter` now goes through `CameraControlManager` with a GUI
      listener supplied by `gui`; `AppInternalsAPIAdapter` now resolves the
      privileged application handle through `AppInternalsManager` instead of
      including `MainApplication.h`; `GuiInternalsAPIAdapter` now goes through
      `GuiInternalsManager` with a GUI listener supplied by `gui`
- [x] Keep privileged GUI adapters explicit and separate
- [ ] Eliminate fake capability composition through null adapters
- [ ] Remove duplication between default and legacy paths

## Host Refresh Semantics

- [x] Add workspace event hooks for object-state and structure refresh
- [x] Connect GUI listeners to the new workspace event types
- [x] Provide legacy `AP::WORKSPACE` wrappers for direct workspace notify calls
- [~] Move workspace/object mutation refresh off direct GUI calls
      `ContextMenu.cpp` and selected `MainWindow_Slots.cpp` paths now use
      workspace notifications; image-viewer open/close now also syncs from
      workspace notifications plus `CImage` state instead of imperative
      `AP.cpp -> gui` calls
- [ ] Review remaining `AppStateManager` usage and classify each call as:
      boundary bridge, workspace event candidate, or direct `gui -> gui`
- [ ] Remove direct GUI refresh calls after object-state mutations in property
      editors and similar GUI tools
- [ ] Decide whether batch workspace/object-state updates need a dedicated
      event, or whether one final `notifyStructureChanged()` remains sufficient

## Legacy Inversion

- [~] Make `AP::` call the real API
      `AP.cpp` now delegates to adapters/services with business logic extracted
      out of the file; legacy image insertion now sets object state and uses
      workspace insertion instead of calling GUI image-viewer helpers directly
- [~] Make `UI::` call the real API
      progress/status/runtime paths are thinner, camera/workspace/plugin-panel
      wrappers in `UI.cpp` now route through `core` managers instead of direct
      GUI host-access helpers; `dpVisionLegacyApi` no longer needs the
      `dpVisionGui` autogen include path, but some raw GUI escape hatches
      still remain in `UI.cpp`, including the consciously preserved legacy
      `UI::PROGRESSBAR::instance()`
- [ ] Keep compatibility overloads in the legacy layer only
- [~] Stop adding new business logic to `AP.cpp` and `UI.cpp`
      `AP.cpp` is in good shape; continue reducing helper logic and raw
      privileged access in `UI.cpp`
- [ ] Update `LEGACY_API_MAP.md` when a migration step is completed

## Plugin Path Cleanup

- [x] Define one default plugin access path
- [x] Define one privileged GUI plugin access path
- [x] Ensure privileged GUI access is opt-in by header/type choice
- [~] Update in-tree plugin examples to demonstrate the intended path
      bundled plugins now use `PluginHostAPIAdapter` / `PluginHostGuiAPIAdapter`
      directly; more examples can still be cleaned up if new ones appear
- [x] Treat `AP::` / `UI::` as the supported public plugin surface while
      keeping them thin

## Verification

- [x] `core` target compiles without active source-level `api` includes
- [ ] default plugin API compiles without `gui`
- [x] `dpVisionGui` target compiles without `dpVision::LegacyApi`
- [x] privileged GUI API compiles with `gui`
- [x] legacy wrappers compile and delegate correctly
- [x] `dpVisionLegacyApi` compiles without the `dpVisionGui` autogen include path
- [x] in-tree legacy plugins may consume `dpVisionLegacyApi` statically without
      redefining `DPVISION_BUILD` (use `DPVISION_LEGACY_API_STATIC`)
- [ ] in-tree example plugins still build
- [x] architecture docs still match reality after each phase
- [ ] `GUI_COORDINATION_RULES.md` still matches reality after each phase
- [x] starter CTest target exists (dpVisionApiTests)
- [x] starter tests cover API boundary contracts and StatusBarManager behavior

## Per-Change Mini Checklist

Use this before finalizing any API-related patch.

- [ ] The change moves logic toward the target dependency graph
- [ ] The change preserves exact behavior for existing plugins
- [ ] The change does not add a new accidental GUI leak to the default API
- [ ] The change preserves plugin access to required `core` data
- [ ] The change does not deepen dependence on `AP::` or `UI::`
- [ ] The change updates docs if it alters the intended migration order

- [x] test hierarchy and priorities are documented in `docs/TESTING_STRATEGY.md`
