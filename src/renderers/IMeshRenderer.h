#include "IPointCloudRenderer.h"

class CBaseObject;
class CPolygonGroup;

class IMeshRenderer : public IPointCloudRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
private:
    void renderPoly(CPolygonGroup &poly);
};
