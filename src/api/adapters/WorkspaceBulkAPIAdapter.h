#pragma once

#include "../interfaces/IWorkspaceBulkAPI.h"
#include "../../core/Workspace.h"

class WorkspaceBulkAPIAdapter : public IWorkspaceBulkAPI {
    CWorkspace* ws_;

public:
    explicit WorkspaceBulkAPIAdapter(CWorkspace* ws = nullptr)
        : ws_(ws ? ws : CWorkspace::instance())
    {
    }

    bool removeAll() override
    {
        return ws_->removeAll();
    }

    bool removeSelected() override
    {
        return ws_->removeChecked();
    }

    void setAllVisible(bool visible) override
    {
        ws_->setAllVisible(visible);
    }
};
