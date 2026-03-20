#pragma once

class IWorkspacePanelListener {
public:
    virtual ~IWorkspacePanelListener() = default;

    virtual void propertiesSelectionChanged(int id) = 0;
    virtual void rebuildWorkspaceTree() = 0;
    virtual void setWorkspaceItemChecked(int id, bool checked) = 0;
    virtual void setWorkspaceItemVisible(int id, bool visible) = 0;
    virtual void setWorkspaceItemLocked(int id, bool locked) = 0;
    virtual void setWorkspaceItemKidsVisible(int id, bool visible) = 0;
};
