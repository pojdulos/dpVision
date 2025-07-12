#include "IAnnotationRenderer.h"

class CBaseObject;
class CAnnotationBezier;

class DPVISION_EXPORT IAnnotationBezierRenderer : public IAnnotationRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
};
