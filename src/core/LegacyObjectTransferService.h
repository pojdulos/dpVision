#pragma once

#include "dll_global.h"

#include <memory>

class CBaseObject;

class DPVISION_EXPORT LegacyObjectTransferService {
public:
    static void moveTo(
        const std::shared_ptr<CBaseObject>& object,
        const std::shared_ptr<CBaseObject>& newParent,
        bool keepPosition = true);

    static void copyTo(
        const std::shared_ptr<CBaseObject>& object,
        const std::shared_ptr<CBaseObject>& newParent,
        bool keepPosition = true);
};
