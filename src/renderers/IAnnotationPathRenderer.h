#pragma once
#include "IAnnotationRenderer.h"

class CBaseObject;
class CAnnotationPath;

class DPVISION_EXPORT IAnnotationPathRenderer : public IAnnotationRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
};
