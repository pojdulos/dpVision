#pragma once
#include "IAnnotationRenderer.h"

class CBaseObject;
class CAnnotationTriangle;

class DPVISION_EXPORT IAnnotationTriangleRenderer : public IAnnotationRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
};
