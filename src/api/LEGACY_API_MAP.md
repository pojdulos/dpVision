# Legacy API Map

This file records the intended migration path from the legacy `AP` / `UI`
namespaces to the newer interface-based API under `src/api/interfaces`.

The goal is:
- keep `AP` and `UI` as compatibility shims,
- move new functionality into explicit interfaces/adapters,
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

`AP::WORKSPACE::SELECTION::*`
- Status: keep during migration
- New home: workspace/selection-facing API extensions

Current workspace-facing replacements already exposed in the new API:
- `IWorkspaceAPI::children()`
- `IWorkspaceAPI::selectedObjects(...)`

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
- Status: legacy wrapper
- New home: `IPluginPanelAPI`

`UI::PROGRESSBAR::*`
- Status: legacy wrapper
- New home: `IProgressAPI`
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

`UI::FILESYSTEM::*`
- Status: legacy wrapper
- New home: `IFileSystemAPI`

String/path helpers in `UI`
- Status: legacy wrapper
- New home: `ITextEncodingAPI` and `IFileSystemAPI`

`UI::CAMERA::*`
- Status: mixed; keep user-facing operations, avoid raw access
- New home: `ICameraAPI`
- Raw internals move to: `IGuiInternalsAPI`

`UI::DOCK::WORKSPACE::*`
- Status: mixed; dock commands may survive temporarily, raw getters are legacy
- New home: dedicated dock APIs or `IGuiInternalsAPI` for privileged code
- Current safe replacements include:
  - `IDockWorkspaceAPI::rebuildTree()`
  - `IDockWorkspaceAPI::setItemVisibleById(...)`

Legacy GUI escape hatches still present in `UI` and intended to shrink:
- `UI::CAMERA::transform()`
- `UI::CAMERA::currentViewer()`
- `UI::DOCK::WORKSPACE::instance()`
- `UI::PLUGINPANEL::mainPanel()`
- `UI::PROGRESSBAR::instance()`

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
- `UI::DOCK::WORKSPACE::getSelectedObjects()`
- `UI::DOCK::WORKSPACE::getCurrentItemObj()`
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

## Plugin-facing adapters

Use for ordinary plugins:
- `AppAPIAdapter`
- `PluginUIAPIAdapter`

Use for privileged GUI-heavy plugins:
- `PluginGuiAPIAdapter`

Do not add new plugin requirements by exposing singleton internals through
legacy namespaces if they can be expressed through an explicit interface first.
