#include "IAnnotationRenderer.h"

class CBaseObject;
class CAnnotationBoundVectors;

class DPVISION_EXPORT IAnnotationBoundVectorsRenderer : public IAnnotationRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
};
