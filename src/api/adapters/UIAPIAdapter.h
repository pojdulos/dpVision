#pragma once
#include "../interfaces/IUIAPI.h"
#include "../adapters/DockWorkspaceAPIAdapter.h"
#include "../adapters/DockHistogramAPIAdapter.h"

class UIAPIAdapter : public IUIAPI {
    DockWorkspaceAPIAdapter dockWorkspaceAdapter_;
    DockHistogramAPIAdapter dockHistogramAdapter_;
public:
    IDockWorkspaceAPI& dockWorkspace() override { return dockWorkspaceAdapter_; }
    IDockHistogramAPI& dockHistogram() override { return dockHistogramAdapter_; }
};
