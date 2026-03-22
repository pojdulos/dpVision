#pragma once

#include "../interfaces/IWorkspaceActivationAPI.h"
#include "../../core/Workspace.h"

class WorkspaceActivationAPIAdapter : public IWorkspaceActivationAPI {
    CWorkspace* ws_;

public:
    explicit WorkspaceActivationAPIAdapter(CWorkspace* ws = nullptr)
        : ws_(ws ? ws : CWorkspace::instance())
    {
    }

    int setCurrentObject(int id) override
    {
        ws_->_objectActivate(id);
        return ws_->_getCurrentModelId();
    }

    int currentObjectId() override
    {
        return ws_->_getCurrentModelId();
    }
};
