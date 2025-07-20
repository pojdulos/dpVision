#include "IBaseObjectRenderer.h"

class CBaseObject;
class CObject;

class DPVISION_EXPORT IObjectRenderer : public IBaseObjectRenderer {
public:
    virtual void renderTransform(const CBaseObject* _obj) override;
    virtual void renderKids(const CBaseObject* _obj) override;
protected:
    void renderBoundingBox(CObject* _obj);
};
