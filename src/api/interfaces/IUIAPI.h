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

class IUIAPI {
public:
    virtual ~IUIAPI() = default;
    virtual IDockWorkspaceAPI& dockWorkspace() = 0;
    virtual IDockHistogramAPI& dockHistogram() = 0;
    virtual IPluginPanelAPI& pluginPanel() = 0;
    virtual IFileDialogAPI& fileDialog() = 0;
    virtual IMessageBoxAPI& messageBox() = 0;
    virtual IStatusBarAPI& statusBar() = 0;
    virtual ICameraAPI& camera() = 0;
    virtual IProgressAPI& progress() = 0;
    virtual IFileSystemAPI& fileSystem() = 0;
    virtual ITextEncodingAPI& textEncoding() = 0;
    virtual void updateAllViews(bool buffered = true) = 0;
    virtual void updateProperties() = 0;
};
