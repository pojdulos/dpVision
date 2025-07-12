#pragma once
#include "IAnnotationRenderer.h"

class CBaseObject;
class CAnnotationVPath;

class DPVISION_EXPORT IAnnotationVPathRenderer : public IAnnotationRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
};
