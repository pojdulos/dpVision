#pragma once

// core/IWorkspaceEvents.h
class IWorkspaceEvents {
public:
    virtual void modelChanged(int modelId) = 0;
    virtual void modelAdded(int modelId) = 0;
    virtual void modelRemoved(int modelId) = 0;
    // ...
    virtual ~IWorkspaceEvents() = default;
};
