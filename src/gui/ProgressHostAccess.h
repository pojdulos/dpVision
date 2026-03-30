#pragma once

#include "../core/dll_global.h"

class ProgressIndicator;
class QString;

class DPVISION_EXPORT ProgressHostAccess {
public:
    static ProgressIndicator* instance();
};
