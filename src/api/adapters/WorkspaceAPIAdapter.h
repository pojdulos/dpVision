// WorkspaceAPIAdapter.h
#pragma once
#include "../interfaces/IWorkspaceAPI.h"
#include "AP.h"
#include "Workspace.h"

class WorkspaceAPIAdapter : public IWorkspaceAPI {
    CWorkspace* ws_;
public:
    WorkspaceAPIAdapter(CWorkspace* ws = nullptr) : ws_(ws ? ws : CWorkspace::instance()) {}

    bool addModel(std::shared_ptr<CModel3D> obj, bool setItCurrent = false) override {
        return AP::WORKSPACE::addModel(obj, setItCurrent);
    }
    bool removeModel(int id) override {
        return AP::WORKSPACE::removeModel(id);
    }
    std::shared_ptr<CModel3D> getModel(int id) override {
        return AP::WORKSPACE::getModel(id);
    }
    std::shared_ptr<CModel3D> getCurrentModel() override {
        return AP::WORKSPACE::getCurrentModel();
    }
    size_t size() override {
        return AP::WORKSPACE::size();
    }
};
