#pragma once

#include "../interfaces/IWorkspaceBulkAPI.h"
#include "../../core/AppStateManager.h"
#include "../../core/Workspace.h"
#include "../../core/WorkspacePanelManager.h"

class WorkspaceBulkAPIAdapter : public IWorkspaceBulkAPI {
    CWorkspace* ws_;

public:
    explicit WorkspaceBulkAPIAdapter(CWorkspace* ws = nullptr)
        : ws_(ws ? ws : CWorkspace::instance())
    {
    }

    bool removeAll() override
    {
        ws_->_removeAllModels();
        WorkspacePanelManager::propertiesSelectionChanged(NO_CURRENT_MODEL);
        WorkspacePanelManager::rebuildWorkspaceTree();
        AppStateManager::updateProperties();
        AppStateManager::changeMenuAfterSelect();
        AppStateManager::updateAllViews();
        return true;
    }

    bool removeSelected() override
    {
        const std::list<int> selection = ws_->getSelection();
        for (std::list<int>::const_reverse_iterator it = selection.rbegin(); it != selection.rend(); ++it) {
            ws_->_objectRemove(*it);
        }
        return true;
    }

    void setAllVisible(bool visible) override
    {
        for (CWorkspace::iterator it = ws_->begin(); it != ws_->end(); ++it) {
            it->second->setSelfVisibility(visible);
            it->second->setKidsVisibility(visible);

            WorkspacePanelManager::setWorkspaceItemVisible(it->first, visible);
            WorkspacePanelManager::setWorkspaceItemKidsVisible(it->first, visible);
        }

        AppStateManager::changeMenuAfterSelect();
        AppStateManager::updateProperties();
        AppStateManager::updateAllViews();
    }
};
