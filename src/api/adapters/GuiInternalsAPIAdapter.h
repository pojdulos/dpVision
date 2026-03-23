#pragma once

#include "../interfaces/IGuiInternalsAPI.h"
#include "../../gui/MainWindow.h"
#include "../../gui/DockWidgetPluginPanel.h"
#include "../../gui/DockWidgetWorkspace.h"
#include "../../gui/GLViewer.h"
#include "../../gui/ProgressIndicator.h"
#include "../../gui/WorkspaceDockHostAccess.h"

class GuiInternalsAPIAdapter : public IGuiInternalsAPI {
public:
    CMainWindow* mainWindow() override {
        return CMainWindow::instance();
    }

    DockWidgetWorkspace* workspaceDock() override {
        if (auto win = CMainWindow::instance()) {
            return win->dockWorkspace;
        }
        return nullptr;
    }

    DockWidgetPluginPanel* pluginPanelHost() override {
        if (auto win = CMainWindow::instance()) {
            return win->dockPluginPanel;
        }
        return nullptr;
    }

    GLViewer* currentViewer() override {
        if (auto win = CMainWindow::instance()) {
            return win->currentViewer();
        }
        return nullptr;
    }

    ProgressIndicator* progressIndicator() override {
        if (auto win = CMainWindow::instance()) {
            return win->progressIndicator;
        }
        return nullptr;
    }

    CTransform* currentCameraTransform() override {
        if (auto viewer = currentViewer()) {
            return &viewer->transform();
        }
        return nullptr;
    }

    QVector<std::shared_ptr<CBaseObject>> selectedWorkspaceObjects() override {
        return WorkspaceDockHostAccess::selectedObjects();
    }

    std::shared_ptr<CBaseObject> currentWorkspaceItem() override {
        return WorkspaceDockHostAccess::currentItem();
    }
};
