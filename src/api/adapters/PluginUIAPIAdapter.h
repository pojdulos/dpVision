#pragma once

// Compatibility facade for plugins that should not need a direct link to dpVisionGui.
// For GUI-coupled plugins prefer PluginGuiAPIAdapter.

#include "../interfaces/IPluginHostAPI.h"
#include "PluginPanelAPIAdapter.h"
#include "FileDialogAPIAdapter.h"
#include "FileSystemAPIAdapter.h"
#include "MessageBoxAPIAdapter.h"
#include "StatusBarAPIAdapter.h"
#include "CameraAPIAdapter.h"
#include "ProgressAPIAdapter.h"
#include "PluginHostAPIAdapter.h"
#include "TextEncodingAPIAdapter.h"
#include "UIAPIAdapter.h"
#include "../../core/AppStateManager.h"

// Transitional facade for older plugins.
//
// New plugin code should use host() for default-safe services. The direct
// pluginPanel/camera/progress methods remain only as compatibility helpers.
class PluginUIAPIAdapter {
    PluginHostAPIAdapter hostApi_;
    UIAPIAdapter uiApi_;
    PluginPanelAPIAdapter pluginPanelAdapter_;
    FileDialogAPIAdapter fileDialogAdapter_;
    MessageBoxAPIAdapter messageBoxAdapter_;
    StatusBarAPIAdapter statusBarAdapter_;
    CameraAPIAdapter cameraAdapter_;
    ProgressAPIAdapter progressAdapter_;
    FileSystemAPIAdapter fileSystemAdapter_;
    TextEncodingAPIAdapter textEncodingAdapter_;

public:
    IPluginHostAPI& host() { return hostApi_; }

    // Legacy compatibility methods. New plugin code should prefer host().
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
