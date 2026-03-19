#pragma once

#include "PluginPanelAPIAdapter.h"
#include "FileDialogAPIAdapter.h"
#include "MessageBoxAPIAdapter.h"
#include "StatusBarAPIAdapter.h"
#include "../UI.h"

class PluginUIAPIAdapter {
    PluginPanelAPIAdapter pluginPanelAdapter_;
    FileDialogAPIAdapter fileDialogAdapter_;
    MessageBoxAPIAdapter messageBoxAdapter_;
    StatusBarAPIAdapter statusBarAdapter_;

public:
    IPluginPanelAPI& pluginPanel() { return pluginPanelAdapter_; }
    IFileDialogAPI& fileDialog() { return fileDialogAdapter_; }
    IMessageBoxAPI& messageBox() { return messageBoxAdapter_; }
    IStatusBarAPI& statusBar() { return statusBarAdapter_; }

    void updateAllViews(bool buffered = true) {
        UI::updateAllViews(buffered);
    }

    void updateProperties() {
        UI::DOCK::PROPERTIES::updateProperties();
    }
};
