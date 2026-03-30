#pragma once

#include "dll_global.h"

class DPVISION_EXPORT AppInternalsManager {
public:
    static void setApplication(void* application) { applicationRef() = application; }
    static void* application() { return applicationRef(); }

private:
    static void*& applicationRef() {
        static void* application = nullptr;
        return application;
    }
};
