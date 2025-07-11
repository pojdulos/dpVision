#pragma once
#include <memory>
#include <vector>

class CModel3D;
class CBaseObject;

class IWorkspaceAPI {
public:
    virtual ~IWorkspaceAPI() = default;
    virtual bool addModel(std::shared_ptr<CModel3D> obj, bool setItCurrent = false) = 0;
    virtual bool removeModel(int id) = 0;
    virtual std::shared_ptr<CModel3D> getModel(int id) = 0;
    virtual std::shared_ptr<CModel3D> getCurrentModel() = 0;
    virtual size_t size() = 0;
    // ...
};
