#pragma once
#include "IAnnotationRenderer.h"

class CBaseObject;
class CAnnotationPoint;

class DPVISION_EXPORT IAnnotationPointRenderer : public IAnnotationRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
};
