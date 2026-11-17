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

class IGuiInternalsAPI {
public:
    virtual ~IGuiInternalsAPI() = default;
    [[deprecated("Privileged raw GUI access. Prefer dedicated host APIs where possible.")]]
    virtual CMainWindow* mainWindow() = 0;
    [[deprecated("Privileged raw GUI access. Prefer IDockWorkspaceAPI where possible.")]]
    virtual DockWidgetWorkspace* workspaceDock() = 0;
    [[deprecated("Privileged raw GUI access. Prefer IPluginPanelAPI where possible.")]]
    virtual DockWidgetPluginPanel* pluginPanelHost() = 0;
    [[deprecated("Privileged raw GUI access. Prefer ICameraControlAPI where possible.")]]
    virtual GLViewer* currentViewer() = 0;
    [[deprecated("Privileged raw GUI access. Prefer IProgressControlAPI where possible.")]]
    virtual ProgressIndicator* progressIndicator() = 0;
    [[deprecated("Privileged raw GUI access. Prefer ICameraControlAPI where possible.")]]
    virtual CTransform* currentCameraTransform() = 0;
    virtual QVector<std::shared_ptr<CBaseObject>> selectedWorkspaceObjects() = 0;
    virtual std::shared_ptr<CBaseObject> currentWorkspaceItem() = 0;
};
