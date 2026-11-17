#pragma once

class IWorkspaceActivationAPI {
public:
    virtual ~IWorkspaceActivationAPI() = default;

    virtual int setCurrentObject(int id) = 0;
    virtual int currentObjectId() = 0;
};
