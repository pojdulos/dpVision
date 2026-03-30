#pragma once

#include <memory>

#include <QVector>

class CBaseObject;
class CMainWindow;
class CTransform;
class GLViewer;
class ProgressIndicator;
class DockWidgetPluginPanel;
class DockWidgetWorkspace;

class IGuiInternalsListener {
public:
    virtual ~IGuiInternalsListener() = default;

    virtual CMainWindow* mainWindow() = 0;
    virtual DockWidgetWorkspace* workspaceDock() = 0;
    virtual DockWidgetPluginPanel* pluginPanelHost() = 0;
    virtual GLViewer* currentViewer() = 0;
    virtual ProgressIndicator* progressIndicator() = 0;
    virtual CTransform* currentCameraTransform() = 0;
    virtual QVector<std::shared_ptr<CBaseObject>> selectedWorkspaceObjects() = 0;
    virtual std::shared_ptr<CBaseObject> currentWorkspaceItem() = 0;
};
