#pragma once

#include "../interfaces/IWorkspaceImageAPI.h"
#include "../../core/Workspace.h"
#include "../../core/Image.h"

class WorkspaceImageAPIAdapter : public IWorkspaceImageAPI {
    CWorkspace* ws_;

public:
    explicit WorkspaceImageAPIAdapter(CWorkspace* ws = nullptr)
        : ws_(ws ? ws : CWorkspace::instance())
    {
    }

    bool addImage(std::shared_ptr<CImage> image, bool show3d = false) override
    {
        if (image == nullptr) {
            return false;
        }

        image->setSelfVisibility(show3d);
        return ws_->_objectAdd(image) != -1;
    }
};
