#pragma once
#include "IAnnotationRenderer.h"

class CBaseObject;
class CAnnotationSetOfVertices;

class DPVISION_EXPORT IAnnotationSetOfVerticesRenderer : public IAnnotationRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
};
