#pragma once
#include "../../core/BaseObject.h"
#include "../../core/Object.h"
#include <memory>
#include <set>
#include <utility>
#include <vector>

class CModel3D;
class QString;

class IWorkspaceAPI {
public:
    virtual ~IWorkspaceAPI() = default;
    virtual std::vector<std::pair<int, std::shared_ptr<CModel3D>>> children() = 0;
    virtual std::vector<std::shared_ptr<CBaseObject>> selectedObjects(
        std::set<CBaseObject::Type> types = {},
        std::shared_ptr<CObject> parent = nullptr) = 0;
    virtual bool addModel(std::shared_ptr<CModel3D> obj, bool setItCurrent = false) = 0;
    virtual bool addObject(std::shared_ptr<CBaseObject> obj, bool setItCurrent = false) = 0;
    virtual bool removeModel(int id) = 0;
    virtual std::shared_ptr<CBaseObject> findId(int id) = 0;
    virtual std::shared_ptr<CModel3D> getModel(int id) = 0;
    virtual std::shared_ptr<CModel3D> getCurrentModel() = 0;
    virtual int getCurrentModelId() = 0;
    virtual std::shared_ptr<CModel3D> loadModel(const QString& path, bool synchronous = true, bool setItCurrent = false) = 0;
    virtual size_t size() = 0;
    // ...
};
