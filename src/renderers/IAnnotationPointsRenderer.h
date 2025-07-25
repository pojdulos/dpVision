#pragma once
#include "IAnnotationRenderer.h"

class CBaseObject;
class CAnnotationPoints;

class DPVISION_EXPORT IAnnotationPointsRenderer : public IAnnotationRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
};
