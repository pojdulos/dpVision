#pragma once

#include <memory>

#include <QtCore/QString>
#include <QtCore/QVector>

class CBaseObject;
class CTransform;
class DockWidgetPluginPanel;
class DockWidgetWorkspace;
class GLViewer;
class QObject;
class QPushButton;
class QWidget;

namespace LegacyUiPrivilegedAccess
{
DockWidgetWorkspace* workspaceDock();
std::shared_ptr<CBaseObject> currentWorkspaceItem();
QVector<std::shared_ptr<CBaseObject>> selectedWorkspaceObjects();

GLViewer* currentViewer();
void screenshot(const QString& path, void* viewer);
CTransform* currentCameraTransform();

DockWidgetPluginPanel* pluginPanelHost();
QWidget* pluginPanelInstance(unsigned int pluginId);

QPushButton* addPluginPanelButton(
    unsigned int pluginId,
    const QString& label,
    QObject* receiver,
    const char* slot,
    int row,
    int col,
    int rspan,
    int cspan);

QPushButton* addPluginPanelButton(
    unsigned int pluginId,
    const QString& name,
    const QString& label,
    int row,
    int col,
    int rspan,
    int cspan);
}
