#include "IAnnotationPyramidRenderer.h"
#include <qopengl.h>
#include "AnnotationPyramid.h"

void IAnnotationPyramidRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationPyramid* obj = (CAnnotationPyramid*)_obj;
    obj->renderSelf();
}
