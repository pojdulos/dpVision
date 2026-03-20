#pragma once

#include "dll_global.h"
#include "interfaces/IWorkspacePanelListener.h"

class DPVISION_EXPORT WorkspacePanelManager {
public:
    static void setListener(IWorkspacePanelListener* listener) { listenerRef() = listener; }
    static IWorkspacePanelListener* listener() { return listenerRef(); }

    static void propertiesSelectionChanged(int id) {
        if (listenerRef()) {
            listenerRef()->propertiesSelectionChanged(id);
        }
    }

    static void rebuildWorkspaceTree() {
        if (listenerRef()) {
            listenerRef()->rebuildWorkspaceTree();
        }
    }

    static void setWorkspaceItemChecked(int id, bool checked) {
        if (listenerRef()) {
            listenerRef()->setWorkspaceItemChecked(id, checked);
        }
    }

    static void setWorkspaceItemVisible(int id, bool visible) {
        if (listenerRef()) {
            listenerRef()->setWorkspaceItemVisible(id, visible);
        }
    }

    static void setWorkspaceItemKidsVisible(int id, bool visible) {
        if (listenerRef()) {
            listenerRef()->setWorkspaceItemKidsVisible(id, visible);
        }
    }

private:
    static IWorkspacePanelListener*& listenerRef() {
        static IWorkspacePanelListener* listener = nullptr;
        return listener;
    }
};
