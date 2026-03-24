#pragma once

#include "../interfaces/IPluginGuiAPI.h"
#include "PluginHostGuiAPIAdapter.h"

// Convenience facade for plugins that intentionally opt into the broader GUI
// surface. Public default plugin entry points remain AP:: and UI::.
class PluginGuiAPIAdapter {
    PluginHostGuiAPIAdapter hostApi_;

public:
    IPluginGuiAPI& host() { return hostApi_; }

    IFileDialogAPI& fileDialog() { return hostApi_.fileDialog(); }
    IMessageBoxAPI& messageBox() { return hostApi_.messageBox(); }
    IStatusBarAPI& statusBar() { return hostApi_.statusBar(); }
    IFileSystemAPI& fileSystem() { return hostApi_.fileSystem(); }
    ITextEncodingAPI& textEncoding() { return hostApi_.textEncoding(); }

    void updateAllViews(bool buffered = true) { hostApi_.updateAllViews(buffered); }
    void updateProperties() { hostApi_.updateProperties(); }

    IAppInternalsAPI& appInternals() { return hostApi_.appInternals(); }
    IGuiInternalsAPI& guiInternals() { return hostApi_.guiInternals(); }
    IDockWorkspaceAPI& dockWorkspace() { return hostApi_.dockWorkspace(); }
    IDockHistogramAPI& dockHistogram() { return hostApi_.dockHistogram(); }
    IPluginPanelAPI& pluginPanel() { return hostApi_.pluginPanel(); }
    ICameraControlAPI& camera() { return hostApi_.camera(); }
    IProgressControlAPI& progress() { return hostApi_.progress(); }
};
