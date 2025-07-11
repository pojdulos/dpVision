#pragma once
#include "IDockWorkspaceAPI.h"

class IUIAPI {
public:
    virtual ~IUIAPI() = default;
    virtual IDockWorkspaceAPI& dockWorkspace() = 0;
    // ... inne podsystemy (PROPERTIES, HISTOGRAM itp.)
};
