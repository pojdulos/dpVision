#pragma once
#include "IAnnotationRenderer.h"

class CBaseObject;
class CAnnotationCurvatureMap;

class DPVISION_EXPORT IAnnotationCurvatureMapRenderer : public IAnnotationRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
};
