#pragma once

#include "../interfaces/IWorkspaceSelectionAPI.h"
#include "../../core/AppStateManager.h"
#include "../../core/Workspace.h"
#include "../../core/WorkspacePanelManager.h"

class WorkspaceSelectionAPIAdapter : public IWorkspaceSelectionAPI {
    CWorkspace* ws_;

public:
    explicit WorkspaceSelectionAPIAdapter(CWorkspace* ws = nullptr)
        : ws_(ws ? ws : CWorkspace::instance())
    {
    }

    void select(int id) override
    {
        ws_->addToSelection(id);
        WorkspacePanelManager::setWorkspaceItemChecked(id, true);
        AppStateManager::updateProperties();
        AppStateManager::updateAllViews();
    }

    void unselect(int id) override
    {
        ws_->removeFromSelection(id);
        WorkspacePanelManager::setWorkspaceItemChecked(id, false);
        AppStateManager::updateProperties();
        AppStateManager::updateAllViews();
    }

    void clear() override
    {
        ws_->clearSelection();
    }

    bool contains(int id) override
    {
        return ws_->inSelection(id);
    }

    std::vector<int> ids() override
    {
        const std::list<int> selection = ws_->getSelection();
        return std::vector<int>(selection.begin(), selection.end());
    }

    std::list<int> ids(
        std::set<CBaseObject::Type> types,
        std::shared_ptr<CObject> parent = nullptr)
    {
        return ws_->getSelection(std::move(types), std::move(parent));
    }

    std::vector<std::shared_ptr<CBaseObject>> objects(
        std::set<CBaseObject::Type> types = {},
        std::shared_ptr<CObject> parent = nullptr) override
    {
        std::vector<std::shared_ptr<CBaseObject>> result;
        auto selected = ws_->getSelected(std::move(types), std::move(parent));
        result.reserve(selected.size());
        for (const auto& object : selected) {
            result.push_back(object);
        }
        return result;
    }

    std::list<std::shared_ptr<CBaseObject>> objectList(
        std::set<CBaseObject::Type> types,
        std::shared_ptr<CObject> parent = nullptr)
    {
        return ws_->getSelected(std::move(types), std::move(parent));
    }

    void setSelectedVisible(bool visible) override
    {
        const std::list<int> selection = ws_->getSelection();
        for (int id : selection) {
            if (std::shared_ptr<CModel3D> model = ws_->_getModel(id)) {
                model->setSelfVisibility(visible);
                WorkspacePanelManager::setWorkspaceItemVisible(id, visible);
            }
        }

        AppStateManager::changeMenuAfterSelect();
        AppStateManager::updateProperties();
        AppStateManager::updateAllViews();
    }
};
