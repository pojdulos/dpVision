#include "IAnnotationRenderer.h"

class CBaseObject;
class CAnnotationBox;

class DPVISION_EXPORT IAnnotationBoxRenderer : public IAnnotationRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
};
