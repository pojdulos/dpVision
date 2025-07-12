#include "IObjectRenderer.h"

class CBaseObject;
class CModel3D;

class IModel3DRenderer : public IObjectRenderer {
public:
    virtual void renderTransform(const CBaseObject* _obj) override;
    virtual void renderSelf(const CBaseObject* _obj) override;
private:
    void renderBoundingBox(CModel3D* _obj);
    void renderAxes();
};
