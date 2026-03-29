#pragma once

#include "dll_global.h"
#include "interfaces/IGuiInternalsListener.h"

class DPVISION_EXPORT GuiInternalsManager {
public:
    static void setListener(IGuiInternalsListener* listener) { listenerRef() = listener; }
    static IGuiInternalsListener* listener() { return listenerRef(); }

    static CMainWindow* mainWindow() {
        return listenerRef() ? listenerRef()->mainWindow() : nullptr;
    }

    static DockWidgetWorkspace* workspaceDock() {
        return listenerRef() ? listenerRef()->workspaceDock() : nullptr;
    }

    static DockWidgetPluginPanel* pluginPanelHost() {
        return listenerRef() ? listenerRef()->pluginPanelHost() : nullptr;
    }

    static GLViewer* currentViewer() {
        return listenerRef() ? listenerRef()->currentViewer() : nullptr;
    }

    static ProgressIndicator* progressIndicator() {
        return listenerRef() ? listenerRef()->progressIndicator() : nullptr;
    }

    static CTransform* currentCameraTransform() {
        return listenerRef() ? listenerRef()->currentCameraTransform() : nullptr;
    }

    static QVector<std::shared_ptr<CBaseObject>> selectedWorkspaceObjects() {
        return listenerRef() ? listenerRef()->selectedWorkspaceObjects() : QVector<std::shared_ptr<CBaseObject>>();
    }

    static std::shared_ptr<CBaseObject> currentWorkspaceItem() {
        return listenerRef() ? listenerRef()->currentWorkspaceItem() : nullptr;
    }

private:
    static IGuiInternalsListener*& listenerRef() {
        static IGuiInternalsListener* listener = nullptr;
        return listener;
    }
};
