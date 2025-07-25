#include "IBaseObjectRenderer.h"

class CBaseObject;

class IPolygonGroupRenderer : public IBaseObjectRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
};
