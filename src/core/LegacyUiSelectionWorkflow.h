#pragma once

#include "AppStateManager.h"
#include "WorkspacePanelManager.h"

// Host-side legacy workflow helpers used by UI:: compatibility wrappers.
// Keep the sequence here so UI.cpp remains a thin facade over application
// services instead of embedding selection orchestration directly.
class LegacyUiSelectionWorkflow {
public:
    static void changeMenuAfterSelect()
    {
        AppStateManager::changeMenuAfterSelect();
    }

    static void updateSelection(int id)
    {
        WorkspacePanelManager::propertiesSelectionChanged(id);
        AppStateManager::changeMenuAfterSelect();
        AppStateManager::updateAllViews();
    }

    static void propertiesSelectionChanged(int id)
    {
        WorkspacePanelManager::propertiesSelectionChanged(id);
    }

    static void updateProperties()
    {
        AppStateManager::updateProperties();
    }
};
