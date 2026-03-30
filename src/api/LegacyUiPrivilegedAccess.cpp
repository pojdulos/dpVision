#include "LegacyUiPrivilegedAccess.h"

#include "../core/CameraControlManager.h"
#include "../core/PluginPanelManager.h"
#include "adapters/GuiInternalsAPIAdapter.h"

#include "GLViewer.h"

namespace
{
GuiInternalsAPIAdapter& guiInternals()
{
    static GuiInternalsAPIAdapter api;
    return api;
}
}

namespace LegacyUiPrivilegedAccess
{
DockWidgetWorkspace* workspaceDock()
{
    return guiInternals().workspaceDock();
}

std::shared_ptr<CBaseObject> currentWorkspaceItem()
{
    return guiInternals().currentWorkspaceItem();
}

QVector<std::shared_ptr<CBaseObject>> selectedWorkspaceObjects()
{
    return guiInternals().selectedWorkspaceObjects();
}

GLViewer* currentViewer()
{
    return guiInternals().currentViewer();
}

void screenshot(const QString& path, void* viewer)
{
    if (viewer != nullptr)
    {
        static_cast<GLViewer*>(viewer)->screenshot(path);
        return;
    }

    CameraControlManager::screenshot(path);
}

CTransform* currentCameraTransform()
{
    return guiInternals().currentCameraTransform();
}

DockWidgetPluginPanel* pluginPanelHost()
{
    return guiInternals().pluginPanelHost();
}

QWidget* pluginPanelInstance(unsigned int pluginId)
{
    return PluginPanelManager::panel(pluginId);
}

QPushButton* addPluginPanelButton(
    unsigned int pluginId,
    const QString& label,
    QObject* receiver,
    const char* slot,
    int row,
    int col,
    int rspan,
    int cspan)
{
    return PluginPanelManager::addButton(pluginId, label, receiver, slot, row, col, rspan, cspan);
}

QPushButton* addPluginPanelButton(
    unsigned int pluginId,
    const QString& name,
    const QString& label,
    int row,
    int col,
    int rspan,
    int cspan)
{
    return PluginPanelManager::addButton(pluginId, name, label, row, col, rspan, cspan);
}
}
