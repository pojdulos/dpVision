#pragma once

#include "dll_global.h"
#include <QVector>
#include <memory>

class CBaseObject;

class DPVISION_EXPORT WorkspaceDockHostAccess {
public:
    static std::shared_ptr<CBaseObject> currentItem();
    static QVector<std::shared_ptr<CBaseObject>> selectedObjects();
};
