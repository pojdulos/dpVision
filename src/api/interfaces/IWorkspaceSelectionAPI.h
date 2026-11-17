#pragma once

#include "../../core/BaseObject.h"
#include "../../core/Object.h"
#include <memory>
#include <set>
#include <vector>

class IWorkspaceSelectionAPI {
public:
    virtual ~IWorkspaceSelectionAPI() = default;

    virtual void select(int id) = 0;
    virtual void unselect(int id) = 0;
    virtual void clear() = 0;
    virtual bool contains(int id) = 0;
    virtual std::vector<int> ids() = 0;
    virtual std::vector<std::shared_ptr<CBaseObject>> objects(
        std::set<CBaseObject::Type> types = {},
        std::shared_ptr<CObject> parent = nullptr) = 0;
    virtual void setSelectedVisible(bool visible) = 0;
};
