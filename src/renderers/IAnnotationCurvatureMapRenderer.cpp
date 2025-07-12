#include "IAnnotationCurvatureMapRenderer.h"
#include <qopengl.h>
#include "AnnotationCurvatureMap.h"

void IAnnotationCurvatureMapRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationCurvatureMap* obj = (CAnnotationCurvatureMap*)_obj;
    obj->renderSelf();
}
