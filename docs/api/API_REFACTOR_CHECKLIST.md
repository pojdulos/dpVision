# API Refactor Checklist

This file is a practical working checklist for implementing the roadmap.

Mark items off as work progresses. If a task is intentionally deferred, note why
in the related commit or PR description.

## Current Status

- [x] Phase 1: remove `core -> api`
- [~] Phase 2: split default API from privileged GUI API
      default and privileged roots exist, compile-time boundary checks are in
      place, and dock/camera/progress/plugin-panel GUI access is explicit;
      remaining work is mostly legacy-surface cleanup in `UI::`
- [~] Phase 5: invert legacy implementation
      `AP.cpp` is now wrapper-oriented and `UI.cpp` has been partially thinned;
      some legacy helper logic still remains in `UI.cpp`

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
      calling `UI::` directly, and selected legacy runtime/status logic was
      moved below `UI.cpp`
- [x] Keep privileged GUI adapters explicit and separate
- [ ] Eliminate fake capability composition through null adapters
- [ ] Remove duplication between default and legacy paths

## Legacy Inversion

- [~] Make `AP::` call the real API
      `AP.cpp` now delegates to adapters/services with business logic extracted
      out of the file
- [~] Make `UI::` call the real API
      progress/status/runtime paths are thinner, but some legacy helper logic
      still remains in `UI.cpp`
- [ ] Keep compatibility overloads in the legacy layer only
- [~] Stop adding new business logic to `AP.cpp` and `UI.cpp`
      `AP.cpp` is in good shape; continue reducing helper logic in `UI.cpp`
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

- [ ] `core` compiles without `api`
- [ ] default plugin API compiles without `gui`
- [ ] privileged GUI API compiles with `gui`
- [x] legacy wrappers compile and delegate correctly
- [ ] in-tree example plugins still build
- [x] architecture docs still match reality after each phase
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
