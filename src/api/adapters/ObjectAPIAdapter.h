// ObjectAPIAdapter.h
#pragma once
#include "../interfaces/IObjectAPI.h"
#include "Workspace.h"

class ObjectAPIAdapter : public IObjectAPI {
public:
    bool remove(std::shared_ptr<CBaseObject> obj) override {
        return CWorkspace::instance()->_objectRemove(obj);
    }
    int addChild(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> child) override {
        if (obj == nullptr || child == nullptr) return -1;
        return CWorkspace::instance()->_objectAdd(child, obj);
    }
    bool removeChild(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> child) override {
        Q_UNUSED(obj);
        return CWorkspace::instance()->_objectRemove(child);
    }
};
