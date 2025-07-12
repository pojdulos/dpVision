#pragma once
#include "IAnnotationRenderer.h"

class CBaseObject;
class CAnnotationCylinder;

class DPVISION_EXPORT IAnnotationCylinderRenderer : public IAnnotationRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
};
