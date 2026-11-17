#pragma once

#include <QObject>

#include "../../core/interfaces/IWorkspacePanelListener.h"
#include "../MainWindow.h"

class QtWorkspacePanelAdapter : public QObject, public IWorkspacePanelListener {
public:
    explicit QtWorkspacePanelAdapter(CMainWindow* window, QObject* parent = nullptr)
        : QObject(parent), window_(window)
    {
    }

    void propertiesSelectionChanged(int id) override
    {
        if (window_ != nullptr && window_->dockProperties != nullptr) {
            window_->dockProperties->raise();
            window_->dockProperties->selectionChanged(id);
            window_->dockProperties->update();
        }
    }

    void rebuildWorkspaceTree() override
    {
        if (window_ != nullptr && window_->dockWorkspace != nullptr) {
            window_->dockWorkspace->rebuildTree();
        }
    }

    void selectWorkspaceItem(int id) override
    {
        if (window_ != nullptr && window_->dockWorkspace != nullptr) {
            window_->dockWorkspace->selectItem(id);
        }
    }

    void setWorkspaceItemChecked(int id, bool checked) override
    {
        if (window_ != nullptr && window_->dockWorkspace != nullptr) {
            window_->dockWorkspace->setItemCheckedById(id, checked);
        }
    }

    void setWorkspaceItemVisible(int id, bool visible) override
    {
        if (window_ != nullptr && window_->dockWorkspace != nullptr) {
            window_->dockWorkspace->setItemVisibleById(id, visible);
        }
    }

    void setWorkspaceItemLocked(int id, bool locked) override
    {
        if (window_ != nullptr && window_->dockWorkspace != nullptr) {
            window_->dockWorkspace->setItemLockedById(id, locked);
        }
    }

    void setWorkspaceItemKidsVisible(int id, bool visible) override
    {
        if (window_ != nullptr && window_->dockWorkspace != nullptr) {
            window_->dockWorkspace->setItemKidsVisibleById(id, visible);
        }
    }

    void setWorkspaceItemLabel(int id, const QString& label) override
    {
        if (window_ != nullptr && window_->dockWorkspace != nullptr) {
            window_->dockWorkspace->setItemLabelById(id, label);
        }
    }

private:
    CMainWindow* window_;
};
