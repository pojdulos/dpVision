// AppAPIAdapter.h
#pragma once
#include "../interfaces/IAppAPI.h"
#include "../adapters/WorkspaceAPIAdapter.h"
#include "../adapters/ModelAPIAdapter.h"
#include "../adapters/ObjectAPIAdapter.h"

class AppAPIAdapter : public IAppAPI {
    WorkspaceAPIAdapter workspaceAPI_;
    ModelAPIAdapter modelAPI_;
    ObjectAPIAdapter objectAPI_;
public:
    IWorkspaceAPI& workspace() override { return workspaceAPI_; }
    IModelAPI& model() override { return modelAPI_; }
    IObjectAPI& object() override { return objectAPI_; }
};
