#pragma once
#include "IDockWorkspaceAPI.h"
#include "IDockHistogramAPI.h"
#include "IPluginPanelAPI.h"
#include "IFileDialogAPI.h"
#include "IMessageBoxAPI.h"
#include "IStatusBarAPI.h"
#include "ICameraAPI.h"
#include "IProgressAPI.h"
#include "IFileSystemAPI.h"
#include "ITextEncodingAPI.h"

// Transitional compatibility aggregate.
//
// This interface mixes default-safe services with GUI-only capabilities and is
// kept only to preserve older plugin code during migration. New code should
// prefer IPluginHostAPI for the default path and IPluginGuiAPI for explicit
// GUI access.
class IUIAPI {
public:
    virtual ~IUIAPI() = default;
    virtual IDockWorkspaceAPI& dockWorkspace() = 0;
    virtual IDockHistogramAPI& dockHistogram() = 0;
    // Legacy mixed-capability access. Prefer IPluginGuiAPI::pluginPanel().
    virtual IPluginPanelAPI& pluginPanel() = 0;
    virtual IFileDialogAPI& fileDialog() = 0;
    virtual IMessageBoxAPI& messageBox() = 0;
    virtual IStatusBarAPI& statusBar() = 0;
    // Legacy mixed-capability access. Prefer IPluginGuiAPI::camera().
    virtual ICameraAPI& camera() = 0;
    // Legacy mixed-capability access. Prefer IPluginGuiAPI::progress().
    virtual IProgressAPI& progress() = 0;
    virtual IFileSystemAPI& fileSystem() = 0;
    virtual ITextEncodingAPI& textEncoding() = 0;
    virtual void updateAllViews(bool buffered = true) = 0;
    virtual void updateProperties() = 0;
};
