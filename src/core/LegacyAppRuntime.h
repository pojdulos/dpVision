#pragma once

#include "dll_global.h"

class DPVISION_EXPORT LegacyAppRuntime {
public:
    static void processEvents(bool immediate = false);
};
