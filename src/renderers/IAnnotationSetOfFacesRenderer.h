#pragma once
#include "IAnnotationRenderer.h"

class CBaseObject;
class CAnnotationSetOfFaces;

class DPVISION_EXPORT IAnnotationSetOfFacesRenderer : public IAnnotationRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
};
