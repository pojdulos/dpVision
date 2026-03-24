#pragma once

#include "../interfaces/IWorkspaceImageAPI.h"
#include "../../core/Workspace.h"
#include "../../core/Image.h"
#include "../../gui/ImageViewerHost.h"

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

    bool addImage(std::shared_ptr<CImage> image, bool showViewer, bool show3d)
    {
        if (showViewer && image != nullptr) {
            ImageViewerHost::open(image.get());
        }

        return addImage(std::move(image), show3d);
    }
};
