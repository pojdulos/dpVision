#pragma once
#include "../interfaces/IDockWorkspaceAPI.h"
#include "../../core/WorkspacePanelManager.h"

class DockWorkspaceAPIAdapter : public IDockWorkspaceAPI {
public:
    void rebuildTree() override {
        WorkspacePanelManager::rebuildWorkspaceTree();
    }

    void setItemVisibleById(int id, bool visible) override {
        WorkspacePanelManager::setWorkspaceItemVisible(id, visible);
    }
};
