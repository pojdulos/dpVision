#include "IObjectRenderer.h"

class CBaseObject;
class CModel3D;

class IModel3DRenderer : public IObjectRenderer {
public:
    virtual void renderTransform(const CBaseObject* _obj) override;
    virtual void renderLast(const CBaseObject* _obj) override;
private:
    void renderAxes();
};
