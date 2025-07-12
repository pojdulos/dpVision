#pragma once
#include "IAnnotationRenderer.h"

class CBaseObject;
class CAnnotationElasticPath;

class DPVISION_EXPORT IAnnotationElasticPathRenderer : public IAnnotationRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
};
