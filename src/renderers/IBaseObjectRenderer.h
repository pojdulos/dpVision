#include "IRenderer.h"

class CBaseObject;

class DPVISION_EXPORT IBaseObjectRenderer : public IRenderer {
public:
    virtual void render(const CBaseObject* obj) override;
    virtual void renderFirst(const CBaseObject* obj) override {};
    virtual void renderTransform(const CBaseObject* obj) override {};
    virtual void renderSelf(const CBaseObject* obj) override {};
    virtual void renderKids(const CBaseObject* obj) override {};
    virtual void renderLast(const CBaseObject* obj) override {};
};
