#pragma once

#include "../interfaces/IPluginGuiAPI.h"
#include "AppInternalsAPIAdapter.h"
#include "DockHistogramAPIAdapter.h"
#include "DockWorkspaceAPIAdapter.h"
#include "GuiPluginPanelAPIAdapter.h"
#include "FileDialogAPIAdapter.h"
#include "FileSystemAPIAdapter.h"
#include "GuiInternalsAPIAdapter.h"
#include "MessageBoxAPIAdapter.h"
#include "StatusBarAPIAdapter.h"
#include "GuiCameraAPIAdapter.h"
#include "GuiProgressAPIAdapter.h"
#include "PluginHostGuiAPIAdapter.h"
#include "TextEncodingAPIAdapter.h"
#include "UIAPIAdapter.h"
#include "../../core/AppStateManager.h"

// Transitional facade for GUI-aware plugins.
//
// New code should prefer host() so the explicit GUI-capable root is visible at
// the call site. The direct methods stay for compatibility with older plugins.
class PluginGuiAPIAdapter {
    PluginHostGuiAPIAdapter hostApi_;
    UIAPIAdapter uiApi_;
    AppInternalsAPIAdapter appInternalsAdapter_;
    GuiInternalsAPIAdapter guiInternalsAdapter_;
    DockWorkspaceAPIAdapter dockWorkspaceAdapter_;
    DockHistogramAPIAdapter dockHistogramAdapter_;
    GuiPluginPanelAPIAdapter pluginPanelAdapter_;
    FileDialogAPIAdapter fileDialogAdapter_;
    MessageBoxAPIAdapter messageBoxAdapter_;
    StatusBarAPIAdapter statusBarAdapter_;
    GuiCameraAPIAdapter cameraAdapter_;
    GuiProgressAPIAdapter progressAdapter_;
    FileSystemAPIAdapter fileSystemAdapter_;
    TextEncodingAPIAdapter textEncodingAdapter_;

public:
    IPluginGuiAPI& host() { return hostApi_; }

    // Legacy compatibility methods. New plugin code should prefer host().
    IAppInternalsAPI& appInternals() { return appInternalsAdapter_; }
    IGuiInternalsAPI& guiInternals() { return guiInternalsAdapter_; }
    IDockWorkspaceAPI& dockWorkspace() { return dockWorkspaceAdapter_; }
    IDockHistogramAPI& dockHistogram() { return dockHistogramAdapter_; }
    IPluginPanelAPI& pluginPanel() { return pluginPanelAdapter_; }
    IFileDialogAPI& fileDialog() { return fileDialogAdapter_; }
    IMessageBoxAPI& messageBox() { return messageBoxAdapter_; }
    IStatusBarAPI& statusBar() { return statusBarAdapter_; }
    ICameraAPI& camera() { return cameraAdapter_; }
    IProgressAPI& progress() { return progressAdapter_; }
    IFileSystemAPI& fileSystem() { return fileSystemAdapter_; }
    ITextEncodingAPI& textEncoding() { return textEncodingAdapter_; }
    // Mixed compatibility aggregate. Prefer host() or explicit GUI methods.
    IUIAPI& ui() { return uiApi_; }

    void updateAllViews(bool buffered = true) {
        AppStateManager::updateAllViews(buffered);
    }

    void updateProperties() {
        AppStateManager::updateProperties();
    }
};
