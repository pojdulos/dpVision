#pragma once
#include "IAnnotationRenderer.h"

class CBaseObject;
class CAnnotationPodkowa;

class DPVISION_EXPORT IAnnotationPodkowaRenderer : public IAnnotationRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
};
