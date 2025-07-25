#include "IObjectRenderer.h"

class CBaseObject;

class IPointCloudRenderer : public IObjectRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;;
};
