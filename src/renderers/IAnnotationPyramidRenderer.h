#pragma once
#include "IAnnotationRenderer.h"

class CBaseObject;
class CAnnotationPyramid;

class DPVISION_EXPORT IAnnotationPyramidRenderer : public IAnnotationRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
};
