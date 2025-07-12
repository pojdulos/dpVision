#include "IAnnotationElasticPathRenderer.h"
#include <qopengl.h>
#include "AnnotationElasticPath.h"

void IAnnotationElasticPathRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationElasticPath* obj = (CAnnotationElasticPath*)_obj;
    obj->renderSelf();
}
