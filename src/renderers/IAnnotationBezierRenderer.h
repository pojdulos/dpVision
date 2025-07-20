#include "IAnnotationRenderer.h"

class CBaseObject;
class CAnnotationBezier;

class DPVISION_EXPORT IAnnotationBezierRenderer : public IAnnotationRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
    void drawTinySpline(CAnnotationBezier *obj);
    void drawBeziers(CAnnotationBezier *obj, int deg, int den, int cut);
};
