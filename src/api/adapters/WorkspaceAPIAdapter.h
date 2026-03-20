// WorkspaceAPIAdapter.h
#pragma once
#include "../interfaces/IWorkspaceAPI.h"
#include "Workspace.h"
#include "Model3D.h"

class WorkspaceAPIAdapter : public IWorkspaceAPI {
    CWorkspace* ws_;
public:
    WorkspaceAPIAdapter(CWorkspace* ws = nullptr) : ws_(ws ? ws : CWorkspace::instance()) {}

    std::vector<std::pair<int, std::shared_ptr<CModel3D>>> children() override {
        std::vector<std::pair<int, std::shared_ptr<CModel3D>>> result;
        result.reserve(ws_->children().size());
        for (const auto& child : ws_->children()) {
            result.push_back(child);
        }
        return result;
    }

    bool addModel(std::shared_ptr<CModel3D> obj, bool setItCurrent = false) override {
        if (obj == nullptr) return false;
        const auto result = ws_->_objectAdd(obj);
        if (setItCurrent) ws_->_objectActivate(obj->id());
        return result != -1;
    }
    bool addObject(std::shared_ptr<CBaseObject> obj, bool setItCurrent = false) override {
        if (obj == nullptr) return false;
        const auto result = ws_->_objectAdd(obj);
        if (setItCurrent) ws_->_objectActivate(obj->id());
        return result != -1;
    }
    bool removeModel(int id) override {
        return ws_->_objectRemove(id);
    }
    std::shared_ptr<CModel3D> getModel(int id) override {
        return ws_->_getModel(id);
    }
    std::shared_ptr<CModel3D> getCurrentModel() override {
        return ws_->_getModel(ws_->_getCurrentModelId());
    }
    int getCurrentModelId() override {
        return ws_->_getCurrentModelId();
    }
    std::shared_ptr<CModel3D> loadModel(const QString& path, bool synchronous = true, bool setItCurrent = false) override {
        auto obj = CModel3D::load(path, synchronous);
        if (obj != nullptr && addModel(obj, setItCurrent)) {
            return obj;
        }
        return nullptr;
    }
    size_t size() override {
        return ws_->size();
    }
};
