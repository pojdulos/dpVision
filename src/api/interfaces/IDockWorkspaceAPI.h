#pragma once
#include <memory>
#include <vector>

class CBaseObject;

class IDockWorkspaceAPI {
public:
    virtual ~IDockWorkspaceAPI() = default;
    virtual void show(bool visible) = 0;
    virtual void rebuildTree() = 0;
    virtual void addItem(int id, int parentId = -1) = 0;
    virtual void addItem(std::shared_ptr<CBaseObject> obj) = 0;
    virtual void removeItem(int id) = 0;
    virtual void selectItem(int id) = 0;
    // ... inne metody
};
