# Testing Strategy

This document defines the initial test hierarchy for dpVision and the first
areas that should be covered while the suite is still small.

## Goals

- keep unit tests cheap to run and easy to locate
- cover stable business logic before GUI-heavy paths
- use integration tests for real file formats and end-to-end host behavior
- add regression tests for every fixed bug that could reasonably reappear

## Test Hierarchy

Mirror `src/` structure where possible.

```text
tests/
  support/          shared test framework, fixtures, helpers, fakes
  unit/
    api/            API contracts, adapters, capability boundaries
    core/           deterministic logic with no GUI dependency
    gui/            GUI-adjacent logic that is testable without click automation
  integration/
    parsers/        sample-based importer/exporter coverage
    workspace/      load/save and host lifecycle scenarios
    plugins/        plugin loading and host/plugin interaction
```

## Current Layout

Implemented starting structure:

- `tests/support/TestFramework.h`
- `tests/support/TestMain.cpp`
- `tests/unit/api/ApiBoundaryTests.cpp`
- `tests/unit/core/BoundingBoxTests.cpp`
- `tests/unit/core/StatusBarManagerTests.cpp`

## Priorities

Recommended order for new tests:

1. stable `unit/core` domain types
2. `unit/api` boundary and capability contracts
3. regression tests for fixed bugs
4. `integration/parsers`
5. `integration/workspace`
6. `unit/gui` only for isolated logic, not full UI automation

This order gives the best return early: stable code gets coverage first, test
runtime stays low, and failures point to small units instead of the whole app.

## First Test Candidates

### `tests/unit/core`

Prefer long-lived domain logic and value-like types over transitional managers or
legacy facades.

1. `CBoundingBox`
   cover invalid/null state, expansion, containment, overlap, ray hit, and axis
   selection logic
2. `CQuaternion`
   cover identity, Euler conversion, inverse, vector rotation, and matrix
   conversion invariants
3. `CMatrix3x3`
   cover determinant and transpose on simple matrices with known results
4. `CTransform`
   cover matrix roundtrip, translation/scale decomposition, and local/world
   conversion once the API-facing coupling around it is stable enough
5. `StatusBarManager`
   keep extending only while it remains an active host-side service
6. `ParserOBJ`, `ParserSTL`, `ParserDPV`
   start with narrow parser-edge tests, then move richer scenarios to
   integration tests over fixture files

Avoid using deprecated or clearly transitional classes as the first wave of
coverage unless they guard critical compatibility behavior.

### Selected First Target: `CBoundingBox`

Why this class first:

- it models stable geometry rules rather than refactor-churn infrastructure
- it has deterministic behavior and low setup cost
- failures should be local and easy to diagnose
- it exercises logic that other systems will depend on transitively

Initial test set:

1. default constructor creates an invalid box
2. `reset(NullBB)` creates a valid null box at the origin
3. expanding an invalid box by points sets correct min/max extents
4. expanding by another box merges extents correctly
5. `contains()` is inclusive on boundaries and rejects outside points
6. `intersection()` returns the exact overlap region
7. disjoint boxes produce an invalid intersection
8. `hit()` returns the expected entry point for an incoming ray
9. `hit()` returns false for a clear miss
10. `longest_axis()` returns the index of the largest span

### `tests/unit/api`

Focus on contract safety and accidental capability leaks:

1. default host API does not expose GUI-only types
2. privileged GUI API exposes only the intended GUI surface
3. adapter calls delegate to the correct host service
4. null or unavailable capabilities fail predictably
5. legacy wrappers `AP::` and `UI::` remain thin and behavior-compatible where
   still supported

### `tests/integration/parsers`

Use `sample_data/` and keep fixtures versioned:

1. DICOM sample directory imports without runtime errors
2. OBJ sample loads expected object counts or key metadata
3. save-load-save roundtrip for supported workspace formats does not lose
   critical metadata
4. invalid or truncated sample files fail with controlled error handling

### `tests/integration/plugins`

1. in-tree example plugin loads through the intended host path
2. privileged plugin path must be explicit and opt-in
3. plugin startup failure is reported cleanly

## Conventions

- Keep one responsibility per test file.
- Name files after the production class or boundary under test.
- Put reusable fakes and fixtures in `tests/support/`.
- Prefer deterministic assertions over snapshot-style output checks.
- Do not start with UI click automation. Test host-side logic first.
- When fixing a bug, add a regression test in the nearest matching folder before
  or together with the fix.

## CMake Guidance

- Keep test target definitions in `tests/CMakeLists.txt`.
- Add new files explicitly instead of using a recursive glob.
- If the suite grows significantly, split into multiple test executables by
  layer, for example `dpVisionCoreTests` and `dpVisionIntegrationTests`.