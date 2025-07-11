// ModelAPIAdapter.h
#pragma once
#include "../interfaces/IModelAPI.h"
#include "AP.h"

class ModelAPIAdapter : public IModelAPI {
public:
    int addChild(std::shared_ptr<CModel3D> obj, std::shared_ptr<CBaseObject> child) override {
        return AP::MODEL::addChild(obj, child);
    }
    void removeChild(std::shared_ptr<CBaseObject> obj, std::shared_ptr<CBaseObject> child) override {
        AP::MODEL::removeChild(obj, child);
    }
    int addAnnotation(std::shared_ptr<CModel3D> obj, std::shared_ptr<CBaseObject> an) override {
        return AP::MODEL::addAnnotation(obj, std::dynamic_pointer_cast<CAnnotation>(an));
    }
};
