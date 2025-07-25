#pragma once
#include "IAnnotationRenderer.h"

class CBaseObject;
class CAnnotationVertex;

class DPVISION_EXPORT IAnnotationVertexRenderer : public IAnnotationRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
};
