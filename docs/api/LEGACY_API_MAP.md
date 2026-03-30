# Legacy API Map

This file records how the public `AP` / `UI` namespaces map to the layered
host-side implementation under `src/api/interfaces`, `src/api/adapters`, and
host access helpers.

The goal is:
- keep `AP` and `UI` as the supported plugin-facing surface,
- implement them through explicit host-side services,
- avoid adding new escape hatches to singleton GUI/application objects.

## AP

`AP::OBJECT::*`
- Status: keep during migration
- New home: `IObjectAPI`

`AP::MODEL::*`
- Status: keep during migration
- New home: `IModelAPI`

`AP::WORKSPACE::*`
- Status: keep during migration
- New home: `IWorkspaceAPI`

Activation replacements:
- `AP::WORKSPACE::setCurrentModel`
- New home: `IWorkspaceActivationAPI`

Bulk workspace replacements:
- `AP::WORKSPACE::{removeAllModels,removeSelectedModels,setAllModelsVisible}`
- New home: `IWorkspaceBulkAPI`

Import replacements:
- `AP::WORKSPACE::loadModel(...)`
- New home: `IWorkspaceImportAPI`

Raw model load replacement:
- `AP::MODEL::load(...)`
- New home: `IModelLoadAPI`

Duplication replacements:
- `AP::WORKSPACE::{duplicateModel,duplicateCurrentModel}`
- New home: `IWorkspaceDuplicationAPI`

Image insertion replacement:
- `AP::WORKSPACE::addImage(...)`
- New home: `IWorkspaceAPI::addModel(...)` plus `CImage` state setup in the
  legacy wrapper
- Compatibility note: legacy `showViewer` remains part of the public wrapper,
  but it is now stored as `CImage` state and respected by GUI reaction to
  workspace notifications rather than by a direct GUI call from `AP.cpp`

`AP::WORKSPACE::SELECTION::*`
- Status: keep during migration
- New home: `IWorkspaceSelectionAPI`

Current workspace-facing replacements already exposed in the new API:
- `IWorkspaceAPI::children()`
- `IWorkspaceAPI::selectedObjects(...)`
- `IWorkspaceSelectionAPI::{select,unselect,clear,contains,ids,objects,setSelectedVisible}`

`AP::processEvents`
- Status: legacy helper
- New home: local Qt/UI helpers where needed

`AP::getExeFilePath`
- Status: compatibility helper
- New home: `IAppAPI::exeFilePath()`

`AP::adjustForCurrentFile`
- Status: keep during migration
- New home: `IAppAPI::adjustForCurrentFile()`

Removed legacy escape hatches:
- `AP::mainApp()`
- `AP::mainWin()`
- `AP::mainWinPtr()`
- `AP::getWorkspace()`
- `AP::getUniqueId()`
- `AP::leaveSelectionMode()`
- `AP::WORKSPACE::instance()`
- `AP::PLUGIN::*`
- `AP::EVENTS::*`

Privileged replacements:
- `IAppInternalsAPI`
- `IGuiInternalsAPI`

## UI

`UI::PLUGINPANEL::*`
- Status: supported plugin surface
- Host-side contract: `IPluginPanelAPI`
- Implementation note: most plugin-panel operations now delegate through
  `PluginPanelManager`; `PluginPanelAPIAdapter` uses the same manager for the
  typed host path, while legacy `UI.cpp` no longer instantiates its own local
  plugin-panel adapter layer.
- Remaining special case:
  - the old `addButton(..., QObject* receiver, const char* slot, ...)`
    overload is still legacy-only, but now also routes through
    `PluginPanelManager` instead of calling the dock directly.

`UI::PROGRESSBAR::*`
- Status: supported plugin surface
- Host-side contract: `IProgressControlAPI`
- Implementation note: `UI::PROGRESSBAR::{init,setValue,setText,hide}` now delegate
  straight to the registered `IProgressListener`; `GuiProgressAPIAdapter`
  remains the typed host-side path for `IProgressControlAPI`.
- Plugin migration note:
  - in-tree plugins should prefer `UI::PROGRESSBAR::{init,setValue,setText,hide}`
    for normal updates
  - when a plugin needs cancel handling, prefer `IProgressListener::useCancelButton(...)`
    over direct `ProgressIndicator` wiring
- Current in-tree status:
  - `parserDP` and `splint-maker` were migrated off `UI::PROGRESSBAR::instance()`
  - they still use other privileged `UI` surfaces, so this does not yet mean
    that the whole plugin can stop linking GUI-related host code
- Legacy escape hatch status:
  - `UI::PROGRESSBAR::instance()` is no longer used by the in-tree plugins and
    is currently disabled in `UI`

`UI::STATUSBAR::*`
- Status: legacy wrapper
- New home: `IStatusBarAPI`
- Implementation note: `UI::STATUSBAR::setText()` now delegates directly
  through `StatusBarManager`; `UI.cpp` no longer instantiates a local status
  adapter.

`UI::MESSAGEBOX::*`
- Status: legacy wrapper
- New home: `IMessageBoxAPI`
- Note: compatibility overloads remain in `UI` because some external plugins
  still build against `UI::MESSAGEBOX::*`.
- Implementation note: legacy calls now route directly through
  `UserMessageManager` with `Modal` policy; the typed `IMessageBoxAPI` path
  remains available separately. This keeps the legacy wrapper thin without
  changing plugin-visible behavior.
- Current policy: direct `MessageBoxManager::{information,warning,error}` calls
  default to `Log`, while legacy `UI::MESSAGEBOX::*` and `IMessageBoxAPI`
  force `Modal` and are additionally logged by the Qt adapter.

`UI::FILECHOOSER::*`
- Status: legacy wrapper
- New home: `IFileDialogAPI`
- Implementation note: non-`QString` overloads delegate to the `QString`
  path, and the `QString` path now calls `QFileDialog` directly; the typed
  `IFileDialogAPI` adapter remains the parallel host-side path.

`UI::FILESYSTEM::*`
- Status: legacy wrapper
- New home: `IFileSystemAPI`

String/path helpers in `UI`
- Status: legacy wrapper
- New home: `ITextEncodingAPI` and `IFileSystemAPI`

`UI::CAMERA::*`
- Status: supported plugin surface for camera operations; avoid raw access
- Host-side contract: `ICameraControlAPI`
- Raw internals move to: `IGuiInternalsAPI`
- Implementation note: current camera adapters no longer call `UI::CAMERA::*`
  directly; legacy `UI::CAMERA::*` wrappers now route safe operations through
  `CameraControlManager`, while raw getters still use privileged GUI internals

`UI::DOCK::WORKSPACE::*`
- Status: mixed; dock commands may survive temporarily, raw getters are legacy
- New home: dedicated dock APIs or `IGuiInternalsAPI` for privileged code
- Current safe replacements include:
  - `IDockWorkspaceAPI::rebuildTree()`
  - `IDockWorkspaceAPI::setItemVisibleById(...)`
  - `WorkspacePanelManager::{selectWorkspaceItem,setWorkspaceItemLabel}`
- Compatibility note:
  - `UI::DOCK::WORKSPACE::currentItem()` and
    `UI::DOCK::WORKSPACE::selectedObjects()` are currently kept so privileged
    plugin adapters can preserve exact pre-refactor behavior on Windows
 - Implementation note:
   - legacy workspace-dock wrappers now route through `WorkspacePanelManager`,
     `DockWorkspaceAPIAdapter`, or `GuiInternalsManager` instead of direct
     GUI host-access helpers

Legacy GUI escape hatches still present in `UI` and intended to shrink:
- `UI::CAMERA::transform()`
- `UI::CAMERA::currentViewer()`
- `UI::DOCK::WORKSPACE::instance()`
- `UI::PLUGINPANEL::mainPanel()`

Current implementation note:
- most of `UI::MESSAGEBOX::*`, `UI::FILECHOOSER::*`, `UI::PLUGINPANEL::*`,
  `UI::PROGRESSBAR::*`, `UI::STATUSBAR::*`, and `UI::DOCK::WORKSPACE::*`
  now behave as thin wrappers over `core` managers, listeners, Qt dialog
  calls, or dedicated adapters rather than owning separate logic in `UI.cpp`
- the remaining raw GUI escape hatches used by `UI.cpp` are now centralized in
  `LegacyUiPrivilegedAccess.*`, so the legacy facade has one explicit place
  where privileged GUI calls are gathered for future cleanup or replacement

Image-viewer state note:
- `CImage` now carries a dedicated `showViewer` state separate from
  `CBaseObject::m_showSelf`
- `m_showSelf` remains the 3D-scene visibility flag
- GUI opens or closes image viewers in response to workspace notifications and
  the `CImage` viewer flag

Already removed as unused or purely local wrappers:
- `UI::adjustGroupBoxHeight()`
- `UI::timeElapsed()`
- `UI::show()`
- `UI::DOCK::updateAll()`
- `UI::DOCK::PROPERTIES::show()`
- `UI::DOCK::WORKSPACE::show()`
- `UI::DOCK::WORKSPACE::rebuildTree()`
- `UI::DOCK::WORKSPACE::addItem()`
- `UI::DOCK::WORKSPACE::removeItem()`
- `UI::DOCK::HISTOGRAM::show()`
- `UI::DOCK::HISTOGRAM::setHistogram()`
- `UI::DOCK::HISTOGRAM::getHistogram()`
- several unused `UI::PLUGINPANEL::*` setter/getter helpers
- `UI::IMAGEVIEWER::setImage()`
- `UI::PICVIEWER::reloadImage()`

External plugins that previously used removed legacy workspace-dock helpers have
been migrated to the new API instead of restoring those wrappers:
- selection access: `IWorkspaceAPI::selectedObjects(...)`
- workspace tree refresh: `IDockWorkspaceAPI::rebuildTree()`

## Plugin-facing headers

Use for ordinary plugins:
- `AP.h`
- `UI.h`

Build note for in-tree CMake plugins:
- when a plugin links the static `dpVision::LegacyApi` target directly, define
  `DPVISION_LEGACY_API_STATIC` for that plugin target so `AP` / `UI` symbols
  are treated as normal static-link symbols rather than `dllimport`

Internal/controlled integration helpers:
- `AppAPIAdapter`
- `PluginHostGuiAPIAdapter`
- explicit host-side interfaces such as `IPluginHostAPI` or `IPluginGuiAPI`

Convenience-only wrapper, not the preferred stable contract:
- `PluginGuiAPIAdapter`

Do not add new plugin requirements by exposing singleton internals through
legacy namespaces if they can be expressed through an explicit interface first.
