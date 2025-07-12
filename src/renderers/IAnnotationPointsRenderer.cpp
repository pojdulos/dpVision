#include "IAnnotationPointsRenderer.h"
#include <qopengl.h>
#include "AnnotationPoints.h"

void IAnnotationPointsRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationPoints* obj = (CAnnotationPoints*)_obj;
    obj->renderSelf();
}
