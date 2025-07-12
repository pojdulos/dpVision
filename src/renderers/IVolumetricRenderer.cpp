#include "IVolumetricRenderer.h"
#include <qopengl.h>
#include "Volumetric.h"

void IVolumetricRenderer::renderSelf(const CBaseObject* _obj)
{
    Volumetric* obj = (Volumetric*)_obj;
    obj->renderSelf();
}
