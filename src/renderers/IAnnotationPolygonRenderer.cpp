#include "IAnnotationPolygonRenderer.h"
#include <qopengl.h>
#include "AnnotationPolygon.h"

void IAnnotationPolygonRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationPolygon* obj = (CAnnotationPolygon*)_obj;
    obj->renderSelf();
}
