#pragma once
#include "IRenderer.h"

class CBaseObject;
class CWorkspace;

class DPVISION_EXPORT IWorkspaceRenderer {
public:
    void render(const CWorkspace* obj);
    void renderKids(const CWorkspace* obj);
    void renderLights(const CWorkspace *_obj, bool perm);
};
