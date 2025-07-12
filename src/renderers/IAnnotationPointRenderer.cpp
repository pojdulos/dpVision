#include "IAnnotationPointRenderer.h"
#include <qopengl.h>
#include "AnnotationPoint.h"

void IAnnotationPointRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationPoint* obj = (CAnnotationPoint*)_obj;
    obj->renderSelf();
}
