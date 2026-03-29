#pragma once

#include "../../core/interfaces/IGuiInternalsListener.h"
#include "../CameraHostAccess.h"
#include "../MainWindow.h"
#include "../PluginPanelHostAccess.h"
#include "../ProgressHostAccess.h"
#include "../WorkspaceDockHostAccess.h"

class QtGuiInternalsAdapter : public IGuiInternalsListener {
public:
    CMainWindow* mainWindow() override {
        return CMainWindow::instance();
    }

    DockWidgetWorkspace* workspaceDock() override {
        return WorkspaceDockHostAccess::host();
    }

    DockWidgetPluginPanel* pluginPanelHost() override {
        return PluginPanelHostAccess::host();
    }

    GLViewer* currentViewer() override {
        return CameraHostAccess::currentViewer();
    }

    ProgressIndicator* progressIndicator() override {
        return ProgressHostAccess::instance();
    }

    CTransform* currentCameraTransform() override {
        return CameraHostAccess::transform();
    }

    QVector<std::shared_ptr<CBaseObject>> selectedWorkspaceObjects() override {
        return WorkspaceDockHostAccess::selectedObjects();
    }

    std::shared_ptr<CBaseObject> currentWorkspaceItem() override {
        return WorkspaceDockHostAccess::currentItem();
    }
};
