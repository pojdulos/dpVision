#include "IAnnotationTriangleRenderer.h"
#include <qopengl.h>
#include "AnnotationTriangle.h"

void IAnnotationTriangleRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationTriangle* obj = (CAnnotationTriangle*)_obj;
    obj->renderSelf();
}
