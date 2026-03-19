#pragma once
#include "../interfaces/IDockWorkspaceAPI.h"

class DockWorkspaceAPIAdapter : public IDockWorkspaceAPI {
public:
    void setItemVisibleById(int id, bool visible) override {
        Q_UNUSED(id);
        Q_UNUSED(visible);
    }
};
