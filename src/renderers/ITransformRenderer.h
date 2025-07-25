#pragma once
#include "dll_global.h"

class CTransform;

class DPVISION_EXPORT ITransformRenderer {
public:
    void render(const CTransform*);
private:
    void renderScrew(const CTransform*, float r = 1., float g = 1., float b = 0.);
};
