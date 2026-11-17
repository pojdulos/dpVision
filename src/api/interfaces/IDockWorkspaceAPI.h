#pragma once
#include <memory>

class CBaseObject;

class IDockWorkspaceAPI {
public:
    virtual ~IDockWorkspaceAPI() = default;
    virtual void rebuildTree() = 0;
    virtual void setItemVisibleById(int id, bool visible) = 0;
    virtual void setItemLockedById(int id, bool locked) = 0;
};
