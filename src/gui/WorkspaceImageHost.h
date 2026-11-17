#pragma once

#include "dll_global.h"
#include <memory>

class CImage;

class DPVISION_EXPORT WorkspaceImageHost {
public:
    static bool addImage(
        std::shared_ptr<CImage> image,
        bool showViewer,
        bool show3d = false);

    static void syncViewerState(int imageId, bool activateIfOpened = false);
};
