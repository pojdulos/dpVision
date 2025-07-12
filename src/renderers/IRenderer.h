// src/renderers/IBaseObjectRenderer.h
#include "dll_global.h"

class CBaseObject;

class DPVISION_EXPORT IRenderer {
public:
    virtual ~IRenderer() = default;
    virtual void render(const CBaseObject* obj) = 0;
    virtual void renderTransform(const CBaseObject* obj) = 0;
    virtual void renderSelf(const CBaseObject* obj) = 0;
    virtual void renderKids(const CBaseObject* obj) = 0;
};
