#pragma once

#include "IWorkspaceAPI.h"
#include "IModelAPI.h"
#include "IObjectAPI.h"

class IAppAPI {
public:
    virtual ~IAppAPI() = default;
    virtual IWorkspaceAPI& workspace() = 0;
    virtual IModelAPI& model() = 0;
    virtual IObjectAPI& object() = 0;
};
