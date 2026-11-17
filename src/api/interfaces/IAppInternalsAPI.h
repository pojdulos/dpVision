#pragma once

class CMainApplication;
class CWorkspace;

class IAppInternalsAPI {
public:
    virtual ~IAppInternalsAPI() = default;
    virtual CMainApplication& application() = 0;
    virtual CWorkspace* workspace() = 0;
};
