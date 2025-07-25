#pragma once
#include "IAnnotationRenderer.h"

class CBaseObject;
class CAnnotationTriple;

class DPVISION_EXPORT IAnnotationTripleRenderer : public IAnnotationRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
    void drawAxes(CAnnotationTriple *obj);
};
