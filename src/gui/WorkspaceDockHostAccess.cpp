#include "WorkspaceDockHostAccess.h"

#include "DockWidgetWorkspace.h"
#include "MainWindow.h"

DockWidgetWorkspace* WorkspaceDockHostAccess::host()
{
    if (auto win = CMainWindow::instance()) {
        return win->dockWorkspace;
    }
    return nullptr;
}

void WorkspaceDockHostAccess::selectItem(int id)
{
    if (auto dock = host()) {
        dock->selectItem(id);
    }
}

std::shared_ptr<CBaseObject> WorkspaceDockHostAccess::currentItem()
{
    if (auto dock = host()) {
        return dock->getCurrentItemObj();
    }
    return nullptr;
}

QVector<std::shared_ptr<CBaseObject>> WorkspaceDockHostAccess::selectedObjects()
{
    if (auto dock = host()) {
        return dock->getSelectedObjects();
    }
    return {};
}

void WorkspaceDockHostAccess::setItemLabelById(int id, const QString& label)
{
    if (auto dock = host()) {
        dock->setItemLabelById(id, label);
    }
}
