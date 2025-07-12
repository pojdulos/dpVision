#include "IImageRenderer.h"
#include <qopengl.h>
#include "Image.h"

void IImageRenderer::renderSelf(const CBaseObject* _obj)
{
    CImage* obj = (CImage*)_obj;
    obj->renderSelf();
}
