#include "IAnnotationRenderer.h"

class CBaseObject;
class CAnnotationCamera;

class DPVISION_EXPORT IAnnotationCameraRenderer : public IAnnotationRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
};
