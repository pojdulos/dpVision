#pragma once
#include "../interfaces/IUIAPI.h"
#include "../adapters/DockWorkspaceAPIAdapter.h"

class UIAPIAdapter : public IUIAPI {
    DockWorkspaceAPIAdapter dockWorkspaceAdapter_;
public:
    IDockWorkspaceAPI& dockWorkspace() override { return dockWorkspaceAdapter_; }
    // ... inne podsystemy
};
