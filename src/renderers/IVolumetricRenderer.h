#pragma once
#include "IObjectRenderer.h"

class CBaseObject;
class Volumetric;

class DPVISION_EXPORT IVolumetricRenderer : public IObjectRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
};
