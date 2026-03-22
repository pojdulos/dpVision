#pragma once

// Compatibility facade for plugins that should not need a direct link to dpVisionGui.
// For GUI-coupled plugins prefer PluginGuiAPIAdapter.

#include "PluginPanelAPIAdapter.h"
#include "FileDialogAPIAdapter.h"
#include "MessageBoxAPIAdapter.h"
#include "StatusBarAPIAdapter.h"
#include "CameraAPIAdapter.h"
#include "ProgressAPIAdapter.h"
#include "UIAPIAdapter.h"
#include "../../core/AppStateManager.h"

class PluginUIAPIAdapter {
    UIAPIAdapter uiApi_;
    PluginPanelAPIAdapter pluginPanelAdapter_;
    FileDialogAPIAdapter fileDialogAdapter_;
    MessageBoxAPIAdapter messageBoxAdapter_;
    StatusBarAPIAdapter statusBarAdapter_;
    CameraAPIAdapter cameraAdapter_;
    ProgressAPIAdapter progressAdapter_;

public:
    IPluginPanelAPI& pluginPanel() { return pluginPanelAdapter_; }
    IFileDialogAPI& fileDialog() { return fileDialogAdapter_; }
    IMessageBoxAPI& messageBox() { return messageBoxAdapter_; }
    IStatusBarAPI& statusBar() { return statusBarAdapter_; }
    ICameraAPI& camera() { return cameraAdapter_; }
    IProgressAPI& progress() { return progressAdapter_; }
    IFileSystemAPI& fileSystem() { return uiApi_.fileSystem(); }
    ITextEncodingAPI& textEncoding() { return uiApi_.textEncoding(); }
    IUIAPI& ui() { return uiApi_; }

    void updateAllViews(bool buffered = true) {
        AppStateManager::updateAllViews(buffered);
    }

    void updateProperties() {
        AppStateManager::updateProperties();
    }
};
