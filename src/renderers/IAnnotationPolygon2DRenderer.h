#pragma once
#include "IAnnotationRenderer.h"

class CBaseObject;
class CAnnotationPolygon2D;

class DPVISION_EXPORT IAnnotationPolygon2DRenderer : public IAnnotationRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
};
