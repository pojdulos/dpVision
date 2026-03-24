# API Refactor Checklist

This file is a practical working checklist for implementing the roadmap.

Mark items off as work progresses. If a task is intentionally deferred, note why
in the related commit or PR description.

## Current Status

- [x] Phase 1: remove `core -> api`
- [~] Phase 2: split default API from privileged GUI API
      default and privileged roots exist, and camera/progress have safe
      control contracts; plugin-panel cleanup is still in progress

## Migration Rule

- [ ] Preserve exact plugin behavior for each refactor step
- [ ] If behavior would change, add host-side support in `core` or `gui`
      instead of changing plugin-visible semantics

## Boundary Cleanup

- [x] Remove all includes of `src/api/*` from `src/core/*`
- [x] Remove local `appApi()` helper singletons from `src/core/*`
- [ ] Verify `src/gui/*` does not depend on `src/api/interfaces/*` just to call
      host functionality
- [ ] Confirm no new code in `core` or `gui` is built on `AP::` or `UI::`

## Interface Split

- [ ] Classify every interface as default, privileged, transitional, or legacy
- [x] Split camera access into safe operations vs raw GUI internals
- [x] Split progress access into progress reporting vs widget internals
- [ ] Move or split `IPluginPanelAPI` out of the default path
- [x] Classify `IPluginPanelAPI` as privileged-only while keeping
      `UI::PLUGINPANEL::*` as the public plugin facade
- [ ] Confirm dock-related APIs are privileged-only or host-internal

## Contract Cleanup

- [ ] Review default interfaces for `QString`
- [ ] Review default interfaces for `QWidget*`
- [ ] Review default interfaces for `QPushButton*`
- [ ] Review default interfaces for `GLViewer*`
- [ ] Review default interfaces for `ProgressIndicator*`
- [ ] Review default interfaces for `void*` GUI escape hatches
- [ ] Keep `core` domain types only where they are intentionally part of the
      supported plugin surface

## Adapter Cleanup

- [ ] Identify adapters that only forward to `AP::` or `UI::`
- [~] Rebuild default adapters directly over `core` where possible
      camera/progress adapters now use shared host-side helpers instead of
      calling `UI::` directly
- [ ] Keep privileged GUI adapters explicit and separate
- [ ] Eliminate fake capability composition through null adapters
- [ ] Remove duplication between default and legacy paths

## Legacy Inversion

- [ ] Make `AP::` call the real API
- [ ] Make `UI::` call the real API
- [ ] Keep compatibility overloads in the legacy layer only
- [ ] Stop adding new business logic to `AP.cpp` and `UI.cpp`
- [ ] Update `LEGACY_API_MAP.md` when a migration step is completed

## Plugin Path Cleanup

- [x] Define one default plugin access path
- [x] Define one privileged GUI plugin access path
- [ ] Ensure privileged GUI access is opt-in by header/type choice
- [~] Update in-tree plugin examples to demonstrate the intended path
      bundled plugins now use `PluginHostAPIAdapter` / `PluginHostGuiAPIAdapter`
      directly; more examples can still be cleaned up if new ones appear
- [x] Treat `AP::` / `UI::` as the supported public plugin surface while
      keeping them thin

## Verification

- [ ] `core` compiles without `api`
- [ ] default plugin API compiles without `gui`
- [ ] privileged GUI API compiles with `gui`
- [ ] legacy wrappers compile and delegate correctly
- [ ] in-tree example plugins still build
- [ ] architecture docs still match reality after each phase

## Per-Change Mini Checklist

Use this before finalizing any API-related patch.

- [ ] The change moves logic toward the target dependency graph
- [ ] The change preserves exact behavior for existing plugins
- [ ] The change does not add a new accidental GUI leak to the default API
- [ ] The change preserves plugin access to required `core` data
- [ ] The change does not deepen dependence on `AP::` or `UI::`
- [ ] The change updates docs if it alters the intended migration order
