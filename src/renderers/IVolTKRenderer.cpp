#include "IVolTKRenderer.h"
#include <qopengl.h>
#include "VolTK.h"

void IVolTKRenderer::renderSelf(const CBaseObject* _obj)
{
    CVolTK* obj = (CVolTK*)_obj;
    obj->renderSelf();
}
