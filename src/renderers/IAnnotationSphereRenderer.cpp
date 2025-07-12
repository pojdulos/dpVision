#include "IAnnotationSphereRenderer.h"
#include <qopengl.h>
#include "AnnotationSphere.h"

void IAnnotationSphereRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationSphere* obj = (CAnnotationSphere*)_obj;
    obj->renderSelf();
}
