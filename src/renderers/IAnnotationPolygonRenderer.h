#pragma once
#include "IAnnotationRenderer.h"

class CBaseObject;
class CAnnotationPolygon;

class DPVISION_EXPORT IAnnotationPolygonRenderer : public IAnnotationRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
};
