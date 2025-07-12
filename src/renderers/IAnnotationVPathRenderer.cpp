#include "IAnnotationVPathRenderer.h"
#include <qopengl.h>
#include "AnnotationVPath.h"

void IAnnotationVPathRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationVPath* obj = (CAnnotationVPath*)_obj;
    obj->renderSelf();
}
