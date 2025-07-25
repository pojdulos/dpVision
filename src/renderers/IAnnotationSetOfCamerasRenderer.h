#pragma once
#include "IAnnotationRenderer.h"

class CBaseObject;
class CAnnotationSetOfCameras;

class DPVISION_EXPORT IAnnotationSetOfCamerasRenderer : public IAnnotationRenderer {
public:
    virtual void renderSelf(const CBaseObject* _obj) override;
};
