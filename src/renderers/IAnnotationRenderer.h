#include "IBaseObjectRenderer.h"

class CBaseObject;
class CAnnotation;

class DPVISION_EXPORT IAnnotationRenderer : public IBaseObjectRenderer {
public:
    virtual void renderTransform(const CBaseObject* _obj) override;
    virtual void renderKids(const CBaseObject* _obj) override;
};
