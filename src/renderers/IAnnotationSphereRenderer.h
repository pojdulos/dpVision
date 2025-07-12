#pragma once
#include "IAnnotationRenderer.h"

class CBaseObject;
class CAnnotationSphere;

class DPVISION_EXPORT IAnnotationSphereRenderer : public IAnnotationRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
};
