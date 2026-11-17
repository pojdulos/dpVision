#pragma once

#include "../interfaces/IGuiInternalsAPI.h"
#include "../../core/GuiInternalsManager.h"

class GuiInternalsAPIAdapter : public IGuiInternalsAPI {
public:
    CMainWindow* mainWindow() override {
        return GuiInternalsManager::mainWindow();
    }

    DockWidgetWorkspace* workspaceDock() override {
        return GuiInternalsManager::workspaceDock();
    }

    DockWidgetPluginPanel* pluginPanelHost() override {
        return GuiInternalsManager::pluginPanelHost();
    }

    GLViewer* currentViewer() override {
        return GuiInternalsManager::currentViewer();
    }

    ProgressIndicator* progressIndicator() override {
        return GuiInternalsManager::progressIndicator();
    }

    CTransform* currentCameraTransform() override {
        return GuiInternalsManager::currentCameraTransform();
    }

    QVector<std::shared_ptr<CBaseObject>> selectedWorkspaceObjects() override {
        return GuiInternalsManager::selectedWorkspaceObjects();
    }

    std::shared_ptr<CBaseObject> currentWorkspaceItem() override {
        return GuiInternalsManager::currentWorkspaceItem();
    }
};
