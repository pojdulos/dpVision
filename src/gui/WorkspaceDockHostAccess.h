#pragma once

#include "dll_global.h"
#include <QVector>
#include <QString>
#include <memory>

class CBaseObject;
class DockWidgetWorkspace;

class DPVISION_EXPORT WorkspaceDockHostAccess {
public:
    static DockWidgetWorkspace* host();
    static void selectItem(int id);
    static std::shared_ptr<CBaseObject> currentItem();
    static QVector<std::shared_ptr<CBaseObject>> selectedObjects();
    static void setItemLabelById(int id, const QString& label);
};
