#pragma once

#include "dll_global.h"

class DPVISION_EXPORT ImageViewerState {
public:
    static bool fitToWindow(int imageId);
    static void setFitToWindow(int imageId, bool fit);
    static void clear(int imageId);
};
