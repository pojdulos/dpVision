# GUI Isolation Audit

This document lists the main GUI-side actors in dpVision, the messages they
need to exchange with the rest of the application, the existing non-GUI
contracts supporting that exchange, and the current isolation status.

Goal:
- verify which GUI areas already have a proper `core`-side abstraction
- identify where GUI still depends on `api` or raw widget access too early
- make the next refactor steps obvious and local

## Status Legend

- `OK` - a meaningful non-GUI contract exists and is already the main path
- `Partial` - some contract exists, but raw GUI access or mixed responsibilities remain
- `Missing` - no clear isolation mechanism exists yet

## Audit Table

| GUI actor / subsystem | Needed communication | Existing contract / mechanism | Status | Notes / gaps | Recommended next step |
| --- | --- | --- | --- | --- | --- |
| `CMainWindow` | app state refresh, global view refresh, menu refresh, file-context refresh | `IAppStateListener`, `AppStateManager`, `QtAppStateAdapter`, direct main-window ownership of docks/views | `Partial` | `AppStateManager` is valid as a bridge from `core/api/plugin -> gui`, but local `gui -> gui` orchestration should not be pushed through extra global layers by default | Keep `AppStateManager` only as the host-side bridge; prefer direct `MainWindow -> dock/view` orchestration inside GUI and use workspace events for workspace/object mutation consequences |
| Status bar | set text, timed text, clear | `IStatusListener`, `StatusBarManager`, `QtStatusBarAdapter`, `IStatusBarAPI` | `OK` | Legacy `UI::STATUSBAR` still exists, but the underlying path is isolated | Keep `StatusBarManager`; do not add a second message/status path |
| Progress indicator | init, set value, set text, hide, cancel callback | `IProgressListener`, `QtProgressAdapter`, `IProgressControlAPI` | `OK` | Raw `ProgressIndicator*` still exists as privileged access only | Keep safe progress on `IProgressListener`; keep raw progress widget behind privileged GUI internals only |
| User messages / message boxes | information, warning, error, question, status-channel routing | `IMessageListener`, `UserMessageManager`, `MessageBoxManager`, `QtMessageBoxAdapter`, `IMessageBoxAPI` | `OK` | Legacy wrapper still exists, but the host-side mechanism is already unified | Continue moving callers toward `UserMessageManager` / `IMessageBoxAPI` |
| Workspace dock | select item, read current item, read selected items, rebuild tree, set visible/locked/labels | `IWorkspacePanelListener`, `WorkspacePanelManager`, `IDockWorkspaceAPI`, `WorkspaceDockHostAccess`, `DockWorkspaceAPIAdapter` | `Partial` | Safe operations exist, but raw `DockWidgetWorkspace*` access is still exposed and some flows still go through legacy wrappers | Prefer `IDockWorkspaceAPI` for plugin-facing safe operations and keep raw dock access privileged-only |
| Properties dock | refresh shown properties, react to selection change, host GUI-aware property widgets supplied by selected-object implementations | `IAppStateListener::updateProperties`, `WorkspacePanelManager::propertiesSelectionChanged`, object-level `has_prop_widget()` / `prop_widget()` / `prop_widget_update()` | `Partial` | The mechanism already works for deliberately GUI-aware plugins. The dock itself should remain GUI-owned. This is not a default plugin capability and does not need to be forced into legacy/default-safe API shape | Document and preserve the current mechanism as a privileged GUI extension path; avoid redesign unless a real limitation appears |
| Histogram dock | repaint / show histogram content | `IHistogramDockListener`, `HistogramDockManager`, `IDockHistogramAPI`, `QtHistogramDockAdapter` | `OK` | The contract is narrow but clear | Keep this shape; use it as a reference for other dock capabilities |
| Plugin panel host | create panel, add/remove widgets, read/edit control values | `IPluginPanelAPI`, `PluginPanelAPIAdapter`, `PluginPanelHostAccess` | `Partial` | Safe operations and raw widget access are mixed in one area; `QWidget*` / `QPushButton*` remain exposed | Split conceptually into safe panel operations vs privileged raw widget escape hatches |
| Camera / viewer control | move, rotate, set view, coordinate transforms, screenshot | `ICameraControlAPI`, `GuiCameraAPIAdapter`, `CameraHostAccess` | `Partial` | Safe camera operations exist, but some plugin flows still rely on raw viewer or transform access | Keep extending `ICameraControlAPI`; avoid adding new `GLViewer*`-based call paths |
| Raw viewer internals (`GLViewer`) | direct viewer pointer, camera transform, OpenGL-local behavior | `IGuiInternalsAPI`, `CameraHostAccess` | `Partial` | Privileged boundary exists, but this remains a large raw escape hatch rather than a capability API | Only use for explicit privileged access; move commonly-needed safe behaviors into typed APIs |
| Image viewer / image dock | open viewer, reload, fit-to-window, internal dock state | `ImageViewerHost`, `ImageViewerState`, legacy `UI::PICVIEWER` | `Missing` | Host helper exists, but there is no equally clear `core/api` contract like progress/status/histogram | Decide whether image viewer is internal GUI only or needs an explicit plugin-facing capability |
| Volumetric image dialog / analysis dialogs | modal interaction, user input, image/volume previews | mostly direct GUI logic | `Missing` | These are still plain GUI workflows without a general non-GUI capability layer | Leave as GUI-local unless a real plugin-facing capability emerges |
| Settings dialog | show/edit app settings | `ISettingsStorage`, `SettingsAPIAdapter` for storage, but dialog itself is GUI-local | `Partial` | Storage is isolated, but the dialog interaction is not modeled as a host capability | Keep dialog GUI-local; expose settings data/services, not the dialog itself |
| Tree select / filter / utility dialogs | modal selection / filter input | mostly direct GUI logic | `Missing` | No dedicated non-GUI contract, and likely not worth one unless plugins need those interactions directly | Keep GUI-local unless a repeated plugin-facing pattern appears |

## Main Findings

### 1. Feedback channels are in relatively good shape

The strongest isolation story already exists for:

- status bar
- progress
- user messages
- histogram dock
- app-state refresh hooks

Important refinement:

- `AppStateManager` should not be treated as the answer to every refresh path
- workspace/object mutation consequences should move toward workspace events
- local `gui -> gui` coordination should often stay direct

These areas already follow the preferred pattern:

- `core` owns the listener/manager contract
- `gui` provides the concrete adapter
- `api` can expose a plugin-facing facade without coupling default plugin paths to widgets

### 2. The biggest remaining weakness is mixed safe-vs-raw GUI exposure

The most visible example is plugin panel support:

- useful safe operations exist
- but raw `QWidget*` and `QPushButton*` access still sit nearby
- the conceptual boundary is therefore present, but not clean

The same issue appears in smaller form for workspace dock and camera/viewer access.

### 3. Properties are already handled by a working GUI-aware extension mechanism

The properties dock does not currently need a redesign just for architectural
purity.

The current model is:

- the dock remains GUI-owned
- selected objects may deliberately provide GUI property widgets through
  `has_prop_widget()`, `prop_widget()`, and `prop_widget_update()`
- this is suitable for plugins that consciously opt into GUI access
- this does not need to be exposed through the default or legacy-safe plugin path

The main architectural point is classification, not replacement:

- treat this as a privileged GUI extension mechanism
- do not mix it into the default-safe API
- do not redesign it unless an actual limitation appears

### 4. Image-viewer areas are still not modeled clearly enough

Image-viewer related operations still rely more on helpers and GUI-local
orchestration than on a clearly classified host capability.

That is the area still needing an explicit decision:

- internal GUI subsystem only, or
- a real plugin-facing capability

### 5. The direct `gui -> api` cleanup pass is largely complete

The original high-priority cleanup target was direct GUI dependence on
plugin-facing API adapters and legacy namespaces.

That pass is now largely complete:

- `src/gui/WinMain.cpp` no longer uses `AppAPIAdapter` for startup model import
- `src/gui/MainWindow_Slots.cpp` now uses GUI-owned workspace import/image
  helpers for local file I/O flows
- `src/gui/ContextMenu.cpp` and `src/gui/propModel.cpp` use workspace
  notifications rather than API wrappers for local mutation refresh
- stale legacy `AP.h` includes were removed from the reviewed GUI files
- the `dpVisionGui` target no longer needs to link `dpVision::LegacyApi`

The remaining GUI-side work is no longer primarily about `gui -> api`
dependencies. It is mostly about:

- separating safe vs raw GUI capabilities
- keeping `AppStateManager` limited to true boundary bridging
- deciding which GUI areas should remain internal-only versus become explicit
  plugin-facing capabilities

## Recommended Refactor Order

Completed cleanup notes:
- `src/gui/WinMain.cpp` no longer uses `AppAPIAdapter` for startup model import;
  startup and TCP model loading now go through the GUI-owned
  `WorkspaceImportHost`
- `src/gui/propModel.cpp` is already aligned with the target direction: it does
  not depend on `src/api` and uses `CWorkspace::notifyObjectStateChanged(...)`
  as the transition-path refresh mechanism after direct model mutations
- `src/gui/ContextMenu.cpp` now relies on workspace notifications for
  workspace/object mutation refresh and no longer carries the stale
  `AppStateManager` include
- `src/gui/MainWindow_Slots.cpp` now uses GUI-owned workspace import/image
  helpers for local file I/O flows instead of a local `AppAPIAdapter`
  singleton
- `src/gui/GLViewer.h`, `src/gui/MainWindow.cpp`, `src/gui/PicViewer.cpp`,
  `src/gui/propDataChild.cpp`, and `src/gui/WorkspaceTreeItem.cpp` no longer
  carry unused legacy `AP.h` includes
- settings bootstrap now registers storage in `core` via
  `SettingsStorageRegistry`, so `src/gui/AppSettings.cpp` no longer depends on
  `AppAPIAdapter`
- `dpVisionGui` builds without linking `dpVision::LegacyApi`

Remaining priorities:

1. Cleanly separate safe plugin-panel operations from raw widget escape hatches.

2. Keep the current properties-widget mechanism as a privileged GUI extension
   path and avoid forcing it into the default-safe plugin API.

3. Make an explicit decision for image-viewer operations.

4. Continue treating `IGuiInternalsAPI` as a privileged escape hatch, not as the
   default model for new plugin-facing work.

5. Apply `GUI_COORDINATION_RULES.md` when deciding whether a refresh path should
   stay on `AppStateManager`, move to workspace events, or become direct
   `gui -> gui` orchestration.

## Short Version

The architecture is already strongest where communication is modeled as:

- `core` listener/manager contract
- `gui` adapter
- `api` facade

The next real gains will come less from moving code between files and more from:

- removing `gui -> api` coupling
- separating safe capabilities from raw GUI escape hatches
- clearly classifying which GUI extension points are deliberately privileged and
  which belong in the default-safe plugin path
