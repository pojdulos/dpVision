#pragma once
#include "IModel3DRenderer.h"

class CBaseObject;
class CImage;

class DPVISION_EXPORT IImageRenderer : public IModel3DRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
};
