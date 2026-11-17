#pragma once

#include "dll_global.h"

class CImage;
class QMdiSubWindow;

class DPVISION_EXPORT ImageViewerHost {
public:
    static QMdiSubWindow* instance(int imageId);
    static bool hasOpenViewer(int imageId);
    static void activateOrOpen(int imageId);
    static void open(CImage* image);
    static void closeAll(int imageId);
    static void reload(int imageId);
    static bool fitToWindow(int imageId);
    static void setFitToWindow(int imageId, bool fit);
};
