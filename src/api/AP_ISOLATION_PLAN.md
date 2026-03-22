# AP Isolation Plan

This document classifies the legacy `AP::` functionality by how cleanly it can
be replaced with explicit, isolating interfaces.

Goal:
- stop treating `AP::` as the design center,
- identify the real semantic services plugins need,
- prepare clear interface targets before rewiring more wrappers.

The main rule remains:
- preserve legacy behavior first,
- introduce explicit interfaces only where the semantics are stable enough,
- keep `AP::` as a compatibility shim over those interfaces.

## Classification

Status labels:
- `Ready`: a clean isolating equivalent already exists or is trivial
- `Next`: good candidate for a new explicit interface now
- `Later`: needs more design work before introducing a clean contract
- `Legacy orchestration`: keep as wrapper logic; do not force into a low-level API

## Ready Now

These legacy calls already map well to explicit APIs or managers.

### App Helpers

- `AP::getExeFilePath`
  - New home: `IAppAPI::exeFilePath()`
  - Status: `Ready`

- `AP::adjustForCurrentFile`
  - New home: `IAppAPI::adjustForCurrentFile()`
  - Status: `Ready`

- `AP::processEvents`
  - New home: local UI/Qt helper, not plugin API
  - Status: `Ready`
  - Note: keep as legacy helper only

### Workspace Read Access

- `AP::WORKSPACE::size`
- `AP::WORKSPACE::findId`
- `AP::WORKSPACE::getModel`
- `AP::WORKSPACE::getCurrentModel`
- `AP::WORKSPACE::getCurrentModelId`
  - New home: `IWorkspaceAPI`
  - Status: `Ready`

### Basic Workspace Mutation

- `AP::WORKSPACE::addModel`
- `AP::WORKSPACE::addObject`
- `AP::WORKSPACE::removeModel`
- `AP::WORKSPACE::removeImage`
- `AP::WORKSPACE::removeCurrentModel`
  - New home: `IWorkspaceAPI`
  - Status: `Ready`
  - Note: these are already close to a clean semantic contract

### Object / Model Graph Basics

- `AP::OBJECT::remove`
- `AP::OBJECT::removeChild`
- `AP::OBJECT::addChild`
- `AP::MODEL::addChild`
- `AP::MODEL::addAnnotation`
- `AP::MODEL::removeChild`
- `AP::MODEL::removeAnnotation`
- `AP::addAnnotation(...)`
  - New home: `IObjectAPI` / `IModelAPI`
  - Status: `Ready`

## Next Interfaces To Introduce

These capabilities have stable semantics, but the current API shape is too
legacy-specific or too mixed.

### Workspace Activation API

Legacy source:
- `AP::WORKSPACE::setCurrentModel`

Problem:
- activation is a first-class workspace capability
- today it is performed through direct `_objectActivate(...)`
- there is no explicit plugin-facing activation contract

Proposed new interface:
- `IWorkspaceActivationAPI`

Proposed methods:
- `int setCurrentObject(int id)`
- `int currentObjectId()`

Status: `Next`

### Workspace Selection API

Legacy source:
- `AP::WORKSPACE::SELECTION::*`

Problem:
- selection is not just read access
- it has UI synchronization side effects
- current replacement surface is incomplete

Proposed new interface:
- `IWorkspaceSelectionAPI`

Proposed methods:
- `void select(int id)`
- `void unselect(int id)`
- `void clear()`
- `bool contains(int id)`
- `std::vector<int> ids()`
- `std::vector<std::shared_ptr<CBaseObject>> objects(...)`
- `void setSelectedVisible(bool visible)`

Status: `Next`

### Workspace Import / Load API

Legacy source:
- `AP::WORKSPACE::loadModel(...)`
- `AP::MODEL::load(...)`

Problem:
- loading currently mixes file parsing and workspace insertion
- overload set is large and legacy-typed
- progress handling is folded into overloads instead of an explicit service

Proposed new interface:
- `IWorkspaceImportAPI`

Proposed methods:
- `std::shared_ptr<CModel3D> loadModel(const QString& path, bool synchronous = true, bool setCurrent = false)`
- future cleanup:
  - one options struct instead of overloads

Status: `Next`

### Workspace Bulk Operations API

Legacy source:
- `AP::WORKSPACE::removeAllModels`
- `AP::WORKSPACE::removeSelectedModels`
- `AP::WORKSPACE::setAllModelsVisible`

Problem:
- these are orchestration-heavy operations
- they include refresh behavior, not just core mutation

Proposed new interface:
- `IWorkspaceBulkAPI`

Proposed methods:
- `bool removeAll()`
- `bool removeSelected()`
- `void setAllVisible(bool visible)`

Status: `Next`

### Workspace Duplication API

Legacy source:
- `AP::WORKSPACE::duplicateModel(...)`
- `AP::WORKSPACE::duplicateCurrentModel()`

Problem:
- duplication is not generic add/remove
- current implementation contains model-copy semantics plus insertion policy

Proposed new interface:
- `IWorkspaceDuplicationAPI`

Proposed methods:
- `std::shared_ptr<CModel3D> duplicateModel(int id)`
- `std::shared_ptr<CModel3D> duplicateModel(std::shared_ptr<CModel3D> model)`
- `std::shared_ptr<CModel3D> duplicateCurrentModel()`

Status: `Next`

## Later: Needs Better Design First

### Image Insertion Policy

Legacy source:
- `AP::WORKSPACE::addImage(im, showViewer, show3d)`

Problem:
- mixes core insertion with GUI viewer creation policy
- `showViewer` is a GUI concern
- `show3d` is domain visibility

Possible direction:
- split into:
  - core insertion API
  - explicit GUI display policy in privileged API

Status: `Later`

### Visibility API Shape

Legacy source:
- `AP::WORKSPACE::setAllModelsVisible`
- `AP::WORKSPACE::SELECTION::setModelsVisible`

Problem:
- visibility changes also synchronize workspace UI state
- currently named in terms of models, but underlying objects are broader

Possible direction:
- fold into `IWorkspaceBulkAPI` and `IWorkspaceSelectionAPI`
- keep host-side synchronization hidden

Status: `Later`

## Legacy Orchestration: Keep As Wrapper Logic

### Object Relocation / Copy

Legacy source:
- `AP::OBJECT::moveTo`
- `AP::OBJECT::copyTo`

Why not force a low-level interface yet:
- they combine:
  - hierarchy mutation
  - annotation/object category rules
  - transform preservation
  - synthetic helper model creation
  - host view refresh

Better direction:
- first extract internal helper functions for:
  - validation
  - preserved-transform wrapping
  - final attach strategy
- only then decide whether any clean plugin-facing service should exist

Status: `Legacy orchestration`

## Recommended Implementation Order

1. Add explicit interfaces for:
   - workspace activation
   - workspace selection
   - workspace import
   - workspace bulk operations
   - workspace duplication

2. Implement them over `core` plus host-side managers where UI sync is needed.

3. Migrate in-tree code to the new interfaces first.

4. Rewire `AP::` wrappers to delegate to those interfaces.

5. Only after that revisit the harder orchestration cases like `moveTo` and
   `copyTo`.

## Decision Notes

The key distinction for `AP::` is:
- some functions represent stable host capabilities,
- some functions are only historical convenience wrappers,
- some functions are actually workflow macros.

Only the first group should become new canonical interfaces.
