#pragma once
#include "IObjectRenderer.h"

class CBaseObject;
class CVolTK;

class DPVISION_EXPORT IVolTKRenderer : public IObjectRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
};
