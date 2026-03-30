#pragma once

#include "../interfaces/IPluginGuiAPI.h"
#include "PluginHostGuiAPIAdapter.h"

// Convenience facade for plugins that intentionally opt into the broader GUI
// surface.
//
// This wrapper exists only to shorten calls like `adapter.camera()` instead of
// `adapter.host().camera()`. It should not be treated as the long-term stable
// public contract for plugins.
//
// Supported public entry points remain AP:: / UI:: for the default path and
// IPluginGuiAPI / PluginHostGuiAPIAdapter for explicit GUI-aware plugins.
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
