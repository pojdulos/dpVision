#pragma once
#include <memory>

class CModel3D;
class CBaseObject;

class IModelAPI {
public:
    virtual ~IModelAPI() = default;
    virtual int addChild(std::shared_ptr<CModel3D> obj, std::shared_ptr<CBaseObject> child) = 0;
    virtual void removeChild(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> child) = 0;
    virtual int addAnnotation(std::shared_ptr<CModel3D> obj, std::shared_ptr<CBaseObject> an) = 0;
    // ...
};
