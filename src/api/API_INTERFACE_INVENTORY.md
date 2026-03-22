# API Interface Inventory

This document classifies the current interfaces and adapters under `src/api`
against the target architecture described in `API_ARCHITECTURE.md`.

Its purpose is operational:
- decide what remains in the default plugin API
- decide what moves to explicit privileged GUI access
- identify migration debt
- give refactoring work a stable queue

## Status Labels

- `Keep`: concept fits the target architecture
- `Keep, clean up`: concept is valid, but contract or implementation must change
- `Move to privileged GUI API`: should exist, but not in the default path
- `Legacy only`: should remain only for compatibility or transition
- `Split`: current interface mixes safe and privileged concerns
- `Replace`: current shape should be retired and replaced with a different one

## Default Plugin API Candidates

These are good candidates for the ordinary plugin-facing API, assuming the
contracts are cleaned up where noted.

### IAppAPI

Status: `Split`

Why:
- application-level service aggregation is useful for plugins
- current contract mixes safe app services with Qt-shaped values

Current issues:
- `QString` in public methods
- lifecycle/status helpers mixed with service access

Direction:
- keep as the main entry point or service root
- replace `QString` with standard string types where practical
- keep only default-safe services here
- move privileged GUI/service escape hatches elsewhere

### IWorkspaceAPI

Status: `Keep, clean up`

Why:
- workspace access is central to plugin functionality
- exposing core-domain objects is acceptable in this project

Current issues:
- `QString` in `loadModel`
- contract is still close to a thin wrapper over existing workspace internals

Direction:
- keep domain-rich access
- move toward semantic operations
- keep it backed by `core`, not by legacy APIs

### IWorkspaceSelectionAPI

Status: `Keep`

Why:
- selection is a stable host capability
- it deserves an explicit contract instead of living only in `AP::WORKSPACE::SELECTION`

Direction:
- keep as a separate service instead of overloading `IWorkspaceAPI`
- hide workspace/UI synchronization behind the adapter

### IWorkspaceActivationAPI

Status: `Keep`

Why:
- current-object activation is a stable workspace capability
- it should not stay embedded only in legacy `AP::WORKSPACE::setCurrentModel`

Direction:
- keep as a separate service
- preserve activation event semantics behind the adapter

### IWorkspaceBulkAPI

Status: `Keep`

Why:
- bulk workspace operations are stable host capabilities
- they should not be hidden in `AP::WORKSPACE::*` forever

Direction:
- keep as a separate service
- hide workspace/UI refresh policy behind the adapter

### IWorkspaceImportAPI

Status: `Keep`

Why:
- loading/importing models is a stable host capability
- it should not stay hidden as an overload cluster inside `IWorkspaceAPI`

Direction:
- keep as a separate service
- let `IWorkspaceAPI::loadModel(...)` remain only as compatibility surface

### IWorkspaceDuplicationAPI

Status: `Keep`

Why:
- model duplication is a stable host capability
- it should not stay embedded in legacy workspace wrappers

Direction:
- keep as a separate service
- keep insertion policy inside the adapter

### IWorkspaceImageAPI

Status: `Keep`

Why:
- image insertion into workspace is a stable host capability
- it should not stay bundled with GUI viewer policy

Direction:
- keep as a separate service
- keep `showViewer` out of the default API
- let legacy `AP::WORKSPACE::addImage(...)` orchestrate viewer opening separately

### IModelLoadAPI

Status: `Keep`

Why:
- raw model loading without workspace insertion is a stable capability
- it should be distinct from workspace import

Direction:
- keep as a small dedicated service
- do not mix it with workspace insertion policy

### IModelAPI

Status: `Keep, clean up`

Why:
- model/object graph operations belong in the default API

Current issues:
- currently very thin and tied to workspace internals

Direction:
- keep if it continues to express meaningful model operations
- merge into workspace/object services if it becomes too small

### IObjectAPI

Status: `Keep, clean up`

Why:
- object graph manipulation is valid default plugin functionality

Current issues:
- implementation currently goes straight into workspace internals

Direction:
- keep as semantic object manipulation API
- reduce duplication with `IModelAPI`

### IFileDialogAPI

Status: `Keep, clean up`

Why:
- ordinary plugins need host-provided file picking

Current issues:
- likely still Qt-shaped in signatures and implementation

Direction:
- keep in default API
- prefer string/path abstractions suitable for plugin code

### IMessageBoxAPI

Status: `Keep, clean up`

Why:
- ordinary plugins need user messaging

Current issues:
- likely Qt-shaped public contract

Direction:
- keep as host messaging API
- maintain policy routing behind the implementation

### IStatusBarAPI

Status: `Keep, clean up`

Why:
- lightweight host feedback belongs in the default plugin path

Direction:
- keep
- prefer standard string types in public signatures

### IProgressAPI

Status: `Split`

Why:
- progress reporting is valid in the default API
- raw progress widget access is not

Current issues:
- `ProgressIndicator* instance()` leaks GUI implementation
- `QString` in the default contract

Direction:
- keep progress operations in the default API
- move raw widget access to privileged GUI internals

Current transition:
- `IProgressControlAPI` is the safe contract
- `IProgressAPI` remains as a compatibility interface for older code

### IFileSystemAPI

Status: `Keep`

Why:
- safe utility service

Direction:
- keep in default API

### ITextEncodingAPI

Status: `Keep`

Why:
- useful compatibility/support service

Direction:
- keep in default API

### ISettingsAPI

Status: `Keep, clean up`

Why:
- plugin/application settings belong in the default API

Current issues:
- Qt-based public surface

Direction:
- keep service concept
- consider narrowing or standardizing value transport later

## Privileged GUI API Candidates

These capabilities should continue to exist, but only behind an explicit,
opt-in GUI-facing plugin path.

### IGuiInternalsAPI

Status: `Keep`

Why:
- this is exactly the privileged escape hatch concept the architecture wants

Direction:
- keep explicit and clearly separated from the default API
- move more raw GUI access here rather than into default interfaces

### IAppInternalsAPI

Status: `Keep, clean up`

Why:
- some plugins may need host internals beyond safe default services

Direction:
- keep explicit and narrow
- ensure it is not transitively available from the default path

### IPluginPanelAPI

Status: `Move to privileged GUI API`

Why:
- direct plugin panel construction is GUI work
- current contract exposes `QWidget*`, `QPushButton*`, `QString`, `QStringList`

Direction:
- either move it entirely to the privileged GUI path
- or split it into:
  - a GUI-neutral declarative panel API for default plugins
  - a raw widget API for privileged plugins

Short-term recommended move:
- classify the current interface as privileged

### ICameraAPI

Status: `Split`

Why:
- camera/view operations are reasonable for plugins
- raw access to `CTransform*`, `GLViewer*`, `QString`, `void* viewer` is not

Direction:
- keep safe camera/view actions in default API
- move raw viewer and transform access to `IGuiInternalsAPI`

Current transition:
- `ICameraControlAPI` is the safe contract
- `ICameraAPI` remains as a compatibility interface for older code

### IDockWorkspaceAPI

Status: `Move to privileged GUI API`

Why:
- direct manipulation of dock widgets is GUI-specific

Direction:
- keep only if plugins truly need dock control
- otherwise leave as GUI-only internal host API

Current transition:
- reachable through `IPluginGuiAPI`
- still present in `IUIAPI` only for compatibility

### IDockHistogramAPI

Status: `Move to privileged GUI API`

Why:
- dock repaint/update is GUI-specific

Direction:
- keep privileged-only if needed

Current transition:
- reachable through `IPluginGuiAPI`
- still present in `IUIAPI` only for compatibility

## Cross-Cutting Problem Interfaces

### IUIAPI

Status: `Replace`

Why:
- it aggregates mixed concerns
- current implementation uses `Null*` placeholders, which means it does not
  describe a real capability boundary

Direction:
- replace with clearer service composition
- ordinary plugins should use a default service root
- privileged plugins should use a separate GUI service root
- keep `IUIAPI` only as a compatibility object returned by transitional
  facades, not as the backing implementation for ordinary services

Recommended replacement shape:
- one default plugin host facade
- one privileged GUI facade
- no mixed null-object aggregate pretending to be a complete UI contract

## Adapter Classification

## Core-backed default adapters

These should survive in some form, but must stop depending on legacy wrappers
or GUI details where possible.

- `AppAPIAdapter`: `Keep, clean up`
- `WorkspaceAPIAdapter`: `Keep, clean up`
- `ModelAPIAdapter`: `Keep, clean up`
- `ObjectAPIAdapter`: `Keep, clean up`
- `SettingsAPIAdapter`: `Keep, clean up`
- `FileSystemAPIAdapter`: `Keep`
- `TextEncodingAPIAdapter`: `Keep`
- `StatusBarAPIAdapter`: `Keep, clean up`
- `MessageBoxAPIAdapter`: `Keep, clean up`

## Legacy-bridging adapters

These are migration debt and should shrink over time.

- `PluginPanelAPIAdapter`: `Legacy only`
- `CameraAPIAdapter`: `Legacy only`
- `ProgressAPIAdapter`: `Split`
- `FileDialogAPIAdapter`: `Keep temporarily, likely still Qt-backed`

## Privileged GUI adapters

These fit the target architecture if they remain explicit.

- `GuiInternalsAPIAdapter`: `Keep`
- `GuiPluginPanelAPIAdapter`: `Keep`
- `GuiCameraAPIAdapter`: `Keep`
- `GuiProgressAPIAdapter`: `Keep`
- `AppInternalsAPIAdapter`: `Keep, clean up`

## Transitional composition facades

These should survive conceptually, but likely in a different shape.

- `PluginUIAPIAdapter`: `Replace`
- `PluginGuiAPIAdapter`: `Keep, clean up`
- `UIAPIAdapter`: `Replace`

## Immediate Refactoring Priorities

Priority 1:
- remove `core -> api`
- stop using plugin-facing adapters inside `core`

Priority 2:
- split default API from privileged GUI API
- reclassify `IPluginPanelAPI`, `ICameraAPI`, and `IProgressAPI`

Priority 3:
- retire `IUIAPI` as the main abstraction
- replace it with explicit default and privileged service roots

Priority 4:
- make `AP::` and `UI::` delegate to the real APIs rather than anchoring
  implementations

## Decision Notes

This inventory intentionally allows rich `core` type access in the default API.
That is not considered an architectural violation for this project.

The main boundary is:
- default plugin API may expose `core`
- default plugin API should not expose `gui`

That distinction should guide all subsequent refactoring.
