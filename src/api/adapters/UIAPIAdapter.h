#pragma once

// Transitional null-object aggregate used only to keep older plugin-facing
// facades source-compatible. New composition should go through PluginHostAPI
// and PluginGuiAPI instead of this mixed IUIAPI wrapper.

#include "../interfaces/IUIAPI.h"
#include "../adapters/DockWorkspaceAPIAdapter.h"
#include "../adapters/DockHistogramAPIAdapter.h"
#include "../adapters/NullPluginPanelAPIAdapter.h"
#include "../adapters/NullFileDialogAPIAdapter.h"
#include "../adapters/MessageBoxAPIAdapter.h"
#include "../adapters/StatusBarAPIAdapter.h"
#include "../adapters/NullCameraAPIAdapter.h"
#include "../adapters/NullProgressAPIAdapter.h"
#include "../adapters/FileSystemAPIAdapter.h"
#include "../adapters/TextEncodingAPIAdapter.h"

class UIAPIAdapter : public IUIAPI {
    DockWorkspaceAPIAdapter dockWorkspaceAdapter_;
    DockHistogramAPIAdapter dockHistogramAdapter_;
    NullPluginPanelAPIAdapter pluginPanelAdapter_;
    NullFileDialogAPIAdapter fileDialogAdapter_;
    MessageBoxAPIAdapter messageBoxAdapter_;
    StatusBarAPIAdapter statusBarAdapter_;
    NullCameraAPIAdapter cameraAdapter_;
    NullProgressAPIAdapter progressAdapter_;
    FileSystemAPIAdapter fileSystemAdapter_;
    TextEncodingAPIAdapter textEncodingAdapter_;
public:
    IDockWorkspaceAPI& dockWorkspace() override { return dockWorkspaceAdapter_; }
    IDockHistogramAPI& dockHistogram() override { return dockHistogramAdapter_; }
    IPluginPanelAPI& pluginPanel() override { return pluginPanelAdapter_; }
    IFileDialogAPI& fileDialog() override { return fileDialogAdapter_; }
    IMessageBoxAPI& messageBox() override { return messageBoxAdapter_; }
    IStatusBarAPI& statusBar() override { return statusBarAdapter_; }
    ICameraAPI& camera() override { return cameraAdapter_; }
    IProgressAPI& progress() override { return progressAdapter_; }
    IFileSystemAPI& fileSystem() override { return fileSystemAdapter_; }
    ITextEncodingAPI& textEncoding() override { return textEncodingAdapter_; }

    void updateAllViews(bool buffered = true) override {
        Q_UNUSED(buffered);
    }

    void updateProperties() override {
    }
};
