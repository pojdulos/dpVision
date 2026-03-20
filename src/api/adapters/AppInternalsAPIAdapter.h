#pragma once

#include "../interfaces/IAppInternalsAPI.h"
#include "../../api/AP.h"

class AppInternalsAPIAdapter : public IAppInternalsAPI {
public:
    CMainApplication& application() override {
        return AP::mainApp();
    }

    CWorkspace* workspace() override {
        return AP::getWorkspace();
    }
};
