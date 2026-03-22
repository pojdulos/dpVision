#pragma once

class IWorkspaceBulkAPI {
public:
    virtual ~IWorkspaceBulkAPI() = default;

    virtual bool removeAll() = 0;
    virtual bool removeSelected() = 0;
    virtual void setAllVisible(bool visible) = 0;
};
