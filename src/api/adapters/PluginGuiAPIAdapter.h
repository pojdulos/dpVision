#pragma once

#include "GuiPluginPanelAPIAdapter.h"
#include "FileDialogAPIAdapter.h"
#include "MessageBoxAPIAdapter.h"
#include "StatusBarAPIAdapter.h"
#include "GuiCameraAPIAdapter.h"
#include "GuiProgressAPIAdapter.h"
#include "UIAPIAdapter.h"
#include "../../core/AppStateManager.h"

class PluginGuiAPIAdapter {
    UIAPIAdapter uiApi_;
    GuiPluginPanelAPIAdapter pluginPanelAdapter_;
    FileDialogAPIAdapter fileDialogAdapter_;
    MessageBoxAPIAdapter messageBoxAdapter_;
    StatusBarAPIAdapter statusBarAdapter_;
    GuiCameraAPIAdapter cameraAdapter_;
    GuiProgressAPIAdapter progressAdapter_;

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
