#include "WorkspaceImageHost.h"

#include "../core/Image.h"
#include "../core/Workspace.h"
#include "ImageViewerHost.h"

bool WorkspaceImageHost::addImage(
    std::shared_ptr<CImage> image,
    bool showViewer,
    bool show3d)
{
    if (image == nullptr) {
        return false;
    }

    if (showViewer) {
        ImageViewerHost::open(image.get());
    }

    image->setSelfVisibility(show3d);
    return CWorkspace::instance()->_objectAdd(image) != -1;
}
