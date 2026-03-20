#pragma once

#include "../interfaces/IAppInternalsAPI.h"
#include "../../gui/MainApplication.h"
#include "../../core/Workspace.h"
#include <QApplication>

class AppInternalsAPIAdapter : public IAppInternalsAPI {
public:
    CMainApplication& application() override {
        return *static_cast<CMainApplication*>(QApplication::instance());
    }

    CWorkspace* workspace() override {
        return CWorkspace::instance();
    }
};
