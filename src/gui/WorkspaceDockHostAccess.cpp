#include "WorkspaceDockHostAccess.h"

#include "DockWidgetWorkspace.h"
#include "MainWindow.h"

std::shared_ptr<CBaseObject> WorkspaceDockHostAccess::currentItem()
{
    if (auto win = CMainWindow::instance()) {
        if (win->dockWorkspace != nullptr) {
            return win->dockWorkspace->getCurrentItemObj();
        }
    }
    return nullptr;
}

QVector<std::shared_ptr<CBaseObject>> WorkspaceDockHostAccess::selectedObjects()
{
    if (auto win = CMainWindow::instance()) {
        if (win->dockWorkspace != nullptr) {
            return win->dockWorkspace->getSelectedObjects();
        }
    }
    return {};
}
