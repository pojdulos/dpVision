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

    image->setShowViewer(showViewer);
    image->setSelfVisibility(show3d);
    return CWorkspace::instance()->_objectAdd(image) != -1;
}

void WorkspaceImageHost::syncViewerState(int imageId, bool activateIfOpened)
{
    std::shared_ptr<CBaseObject> object = CWorkspace::instance()->getSomethingWithId(imageId);
    std::shared_ptr<CImage> image = std::dynamic_pointer_cast<CImage>(object);
    if (image == nullptr) {
        return;
    }

    if (image->getShowViewer()) {
        if (ImageViewerHost::hasOpenViewer(imageId)) {
            ImageViewerHost::reload(imageId);
        }
        else if (activateIfOpened) {
            ImageViewerHost::activateOrOpen(imageId);
        }
        else {
            ImageViewerHost::open(image.get());
        }
        return;
    }

    if (ImageViewerHost::hasOpenViewer(imageId)) {
        ImageViewerHost::closeAll(imageId);
    }
}
