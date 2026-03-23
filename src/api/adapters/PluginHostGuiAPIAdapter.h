#pragma once

#include "../interfaces/IPluginGuiAPI.h"
#include "AppInternalsAPIAdapter.h"
#include "DockHistogramAPIAdapter.h"
#include "DockWorkspaceAPIAdapter.h"
#include "GuiCameraAPIAdapter.h"
#include "GuiInternalsAPIAdapter.h"
#include "GuiPluginPanelAPIAdapter.h"
#include "GuiProgressAPIAdapter.h"
#include "PluginHostAPIAdapter.h"

class PluginHostGuiAPIAdapter : public IPluginGuiAPI {
    PluginHostAPIAdapter hostAdapter_;
    AppInternalsAPIAdapter appInternalsAdapter_;
    GuiInternalsAPIAdapter guiInternalsAdapter_;
    DockWorkspaceAPIAdapter dockWorkspaceAdapter_;
    DockHistogramAPIAdapter dockHistogramAdapter_;
    GuiPluginPanelAPIAdapter pluginPanelAdapter_;
    GuiCameraAPIAdapter cameraAdapter_;
    GuiProgressAPIAdapter progressAdapter_;

public:
    IFileDialogAPI& fileDialog() override { return hostAdapter_.fileDialog(); }
    IMessageBoxAPI& messageBox() override { return hostAdapter_.messageBox(); }
    IStatusBarAPI& statusBar() override { return hostAdapter_.statusBar(); }
    IFileSystemAPI& fileSystem() override { return hostAdapter_.fileSystem(); }
    ITextEncodingAPI& textEncoding() override { return hostAdapter_.textEncoding(); }

    void updateAllViews(bool buffered = true) override {
        hostAdapter_.updateAllViews(buffered);
    }

    void updateProperties() override {
        hostAdapter_.updateProperties();
    }

    IAppInternalsAPI& appInternals() override { return appInternalsAdapter_; }
    IGuiInternalsAPI& guiInternals() override { return guiInternalsAdapter_; }
    IDockWorkspaceAPI& dockWorkspace() override { return dockWorkspaceAdapter_; }
    IDockHistogramAPI& dockHistogram() override { return dockHistogramAdapter_; }
    IPluginPanelAPI& pluginPanel() override { return pluginPanelAdapter_; }
    ICameraControlAPI& camera() override { return cameraAdapter_; }
    IProgressControlAPI& progress() override { return progressAdapter_; }
};
