#pragma once

#include "../interfaces/IWorkspaceSelectionAPI.h"
#include "../../core/Workspace.h"

class WorkspaceSelectionAPIAdapter : public IWorkspaceSelectionAPI {
    CWorkspace* ws_;

public:
    explicit WorkspaceSelectionAPIAdapter(CWorkspace* ws = nullptr)
        : ws_(ws ? ws : CWorkspace::instance())
    {
    }

    void select(int id) override
    {
        ws_->setChecked(id, true);
    }

    void unselect(int id) override
    {
        ws_->setChecked(id, false);
    }

    void clear() override
    {
        ws_->clearChecked();
        ws_->notifyStructureChanged();
    }

    bool contains(int id) override
    {
        return ws_->isChecked(id);
    }

    std::vector<int> ids() override
    {
        const std::list<int> selection = ws_->checkedIds();
        return std::vector<int>(selection.begin(), selection.end());
    }

    std::list<int> ids(
        std::set<CBaseObject::Type> types,
        std::shared_ptr<CObject> parent = nullptr)
    {
        return ws_->checkedIds(std::move(types), std::move(parent));
    }

    std::vector<std::shared_ptr<CBaseObject>> objects(
        std::set<CBaseObject::Type> types = {},
        std::shared_ptr<CObject> parent = nullptr) override
    {
        std::vector<std::shared_ptr<CBaseObject>> result;
        auto selected = ws_->checkedObjects(std::move(types), std::move(parent));
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
        return ws_->checkedObjects(std::move(types), std::move(parent));
    }

    void setSelectedVisible(bool visible) override
    {
        ws_->setCheckedVisible(visible);
    }
};
