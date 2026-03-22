#pragma once

#include "../interfaces/IWorkspaceDuplicationAPI.h"
#include "WorkspaceAPIAdapter.h"

class WorkspaceDuplicationAPIAdapter : public IWorkspaceDuplicationAPI {
    WorkspaceAPIAdapter workspaceApi_;

public:
    std::shared_ptr<CModel3D> duplicateModel(std::shared_ptr<CModel3D> original) override
    {
        if (original == nullptr) {
            return nullptr;
        }

        std::shared_ptr<CModel3D> copy = std::dynamic_pointer_cast<CModel3D>(original->getCopy());
        if (copy != nullptr && workspaceApi_.addModel(copy)) {
            return copy;
        }
        return nullptr;
    }

    std::shared_ptr<CModel3D> duplicateModel(int id) override
    {
        return duplicateModel(workspaceApi_.getModel(id));
    }

    std::shared_ptr<CModel3D> duplicateCurrentModel() override
    {
        return duplicateModel(workspaceApi_.getCurrentModel());
    }
};
