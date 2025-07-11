#pragma once
#include <memory>

class CBaseObject;

class IObjectAPI {
public:
    virtual ~IObjectAPI() = default;
    virtual bool remove(std::shared_ptr<CBaseObject> obj, bool deleteIt = false) = 0;
    virtual int addChild(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> child) = 0;
    virtual bool removeChild(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> child, bool deleteIt = false) = 0;
    // ...
};
