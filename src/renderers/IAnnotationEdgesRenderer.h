#pragma once
#include "IAnnotationRenderer.h"

class CBaseObject;
class CAnnotationEdges;

class DPVISION_EXPORT IAnnotationEdgesRenderer : public IAnnotationRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
};
