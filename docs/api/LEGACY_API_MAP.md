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
- New home: `IWorkspaceImageAPI`
- Compatibility note: legacy `showViewer` behavior remains in `AP::` because it
  is GUI policy, not default workspace capability

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
  `PluginPanelAPIAdapter` and `PluginPanelHostAccess`.
- Remaining special case:
  - the old `addButton(..., QObject* receiver, const char* slot, ...)`
    overload is still legacy-only, but now also routes through
    `PluginPanelHostAccess` instead of calling the dock directly.

`UI::PROGRESSBAR::*`
- Status: supported plugin surface
- Host-side contract: `IProgressControlAPI`
- Implementation note: `UI::PROGRESSBAR::{init,setValue,setText,hide}` now delegate
  primarily through `IProgressListener::getDefault()`. `instance()` remains a
  legacy GUI escape hatch.

`UI::STATUSBAR::*`
- Status: legacy wrapper
- New home: `IStatusBarAPI`
- Implementation note: `UI::STATUSBAR::setText()` now delegates through
  `StatusBarManager`.

`UI::MESSAGEBOX::*`
- Status: legacy wrapper
- New home: `IMessageBoxAPI`
- Note: compatibility overloads remain in `UI` because some external plugins
  still build against `UI::MESSAGEBOX::*`.
- Implementation note: legacy calls now route through `MessageBoxManager`,
  which in turn delegates to the newer `UserMessageManager` / `IMessageListener`
  mechanism. This is the basis for policy-based routing to message box, status
  bar, or log without changing call sites in core/plugins.
- Current policy: direct `MessageBoxManager::{information,warning,error}` calls
  default to `Log`, while legacy `UI::MESSAGEBOX::*` and `IMessageBoxAPI`
  force `Modal` and are additionally logged by the Qt adapter.

`UI::FILECHOOSER::*`
- Status: legacy wrapper
- New home: `IFileDialogAPI`
- Implementation note: non-`QString` overloads now delegate to the `QString`
  path so the legacy namespace has only one real file-dialog execution path.

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
  directly; they share host-side camera access helpers with the GUI-aware path

`UI::DOCK::WORKSPACE::*`
- Status: mixed; dock commands may survive temporarily, raw getters are legacy
- New home: dedicated dock APIs or `IGuiInternalsAPI` for privileged code
- Current safe replacements include:
  - `IDockWorkspaceAPI::rebuildTree()`
  - `IDockWorkspaceAPI::setItemVisibleById(...)`
  - `WorkspaceDockHostAccess::{selectItem,currentItem,selectedObjects,setItemLabelById}`
- Compatibility note:
  - `UI::DOCK::WORKSPACE::currentItem()` and
    `UI::DOCK::WORKSPACE::selectedObjects()` are currently kept so privileged
    plugin adapters can preserve exact pre-refactor behavior on Windows

Legacy GUI escape hatches still present in `UI` and intended to shrink:
- `UI::CAMERA::transform()`
- `UI::CAMERA::currentViewer()`
- `UI::DOCK::WORKSPACE::instance()`
- `UI::PLUGINPANEL::mainPanel()`
- `UI::PROGRESSBAR::instance()`

Current implementation note:
- most of `UI::MESSAGEBOX::*`, `UI::FILECHOOSER::*`, `UI::PLUGINPANEL::*`,
  and `UI::DOCK::WORKSPACE::*` now behave as thin wrappers over adapters or
  host-side GUI helpers rather than owning separate logic in `UI.cpp`

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

Internal/controlled integration helpers:
- `AppAPIAdapter`
- `PluginGuiAPIAdapter`
- explicit host-side interfaces such as `IPluginHostAPI` or `IPluginGuiAPI`

Do not add new plugin requirements by exposing singleton internals through
legacy namespaces if they can be expressed through an explicit interface first.
