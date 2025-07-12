#include "IAnnotationRenderer.h"

class CBaseObject;
class CAnnotationPlane;

class DPVISION_EXPORT IAnnotationPlaneRenderer : public IAnnotationRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
};
