#pragma once

#include "IAppInternalsAPI.h"
#include "ICameraControlAPI.h"
#include "IDockHistogramAPI.h"
#include "IDockWorkspaceAPI.h"
#include "IGuiInternalsAPI.h"
#include "IPluginHostAPI.h"
#include "IPluginPanelAPI.h"
#include "IProgressControlAPI.h"

class IPluginGuiAPI : public IPluginHostAPI {
public:
    ~IPluginGuiAPI() override = default;

    virtual IAppInternalsAPI& appInternals() = 0;
    virtual IGuiInternalsAPI& guiInternals() = 0;
    virtual IDockWorkspaceAPI& dockWorkspace() = 0;
    virtual IDockHistogramAPI& dockHistogram() = 0;
    virtual IPluginPanelAPI& pluginPanel() = 0;
    virtual ICameraControlAPI& camera() = 0;
    virtual IProgressControlAPI& progress() = 0;
};
