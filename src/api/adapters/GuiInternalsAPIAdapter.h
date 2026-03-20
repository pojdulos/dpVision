#pragma once

#include "../interfaces/IGuiInternalsAPI.h"
#include "../../api/AP.h"
#include "../../gui/MainWindow.h"
#include "../../gui/DockWidgetPluginPanel.h"
#include "../../gui/DockWidgetWorkspace.h"
#include "../../gui/GLViewer.h"
#include "../../gui/ProgressIndicator.h"

class GuiInternalsAPIAdapter : public IGuiInternalsAPI {
public:
    CMainWindow* mainWindow() override {
        return AP::mainWinPtr();
    }

    DockWidgetWorkspace* workspaceDock() override {
        if (auto win = AP::mainWinPtr()) {
            return win->dockWorkspace;
        }
        return nullptr;
    }

    DockWidgetPluginPanel* pluginPanelHost() override {
        if (auto win = AP::mainWinPtr()) {
            return win->dockPluginPanel;
        }
        return nullptr;
    }

    GLViewer* currentViewer() override {
        if (auto win = AP::mainWinPtr()) {
            return win->currentViewer();
        }
        return nullptr;
    }

    ProgressIndicator* progressIndicator() override {
        if (auto win = AP::mainWinPtr()) {
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
        if (auto dock = workspaceDock()) {
            return dock->getSelectedObjects();
        }
        return {};
    }

    std::shared_ptr<CBaseObject> currentWorkspaceItem() override {
        if (auto dock = workspaceDock()) {
            return dock->getCurrentItemObj();
        }
        return nullptr;
    }
};
