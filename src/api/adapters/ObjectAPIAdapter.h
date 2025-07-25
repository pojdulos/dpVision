// ObjectAPIAdapter.h
#pragma once
#include "../interfaces/IObjectAPI.h"
#include "../api/AP.h"

class ObjectAPIAdapter : public IObjectAPI {
public:
    bool remove(std::shared_ptr<CBaseObject> obj, bool deleteIt = false) override {
        return AP::OBJECT::remove(obj, deleteIt);
    }
    int addChild(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> child) override {
        return AP::OBJECT::addChild(obj, child);
    }
    bool removeChild(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> child, bool deleteIt = false) override {
        return AP::OBJECT::removeChild(obj, child, deleteIt);
    }
};
