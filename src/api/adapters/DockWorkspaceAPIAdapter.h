#pragma once
#include "../interfaces/IDockWorkspaceAPI.h"
#include "UI.h"

class DockWorkspaceAPIAdapter : public IDockWorkspaceAPI {
public:
    void show(bool visible) override {
        UI::DOCK::WORKSPACE::show(visible);
    }
    void rebuildTree() override {
        UI::DOCK::WORKSPACE::rebuildTree();
    }
    void addItem(int id, int parentId = -1) override {
        UI::DOCK::WORKSPACE::addItem(id, parentId);
    }
    void addItem(std::shared_ptr<CBaseObject> obj) override {
        UI::DOCK::WORKSPACE::addItem(obj);
    }
    void removeItem(int id) override {
        UI::DOCK::WORKSPACE::removeItem(id);
    }
    void selectItem(int id) override {
        UI::DOCK::WORKSPACE::selectItem(id);
    }
    // ... inne metody
};
