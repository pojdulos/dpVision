#pragma once

#include "../interfaces/IAppInternalsAPI.h"
#include "../../core/AppInternalsManager.h"
#include "../../core/Workspace.h"

class AppInternalsAPIAdapter : public IAppInternalsAPI {
public:
    CMainApplication& application() override {
        return *static_cast<CMainApplication*>(AppInternalsManager::application());
    }

    CWorkspace* workspace() override {
        return CWorkspace::instance();
    }
};
