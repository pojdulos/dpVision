#include "IAnnotationPathRenderer.h"
#include <qopengl.h>
#include "AnnotationPath.h"

void IAnnotationPathRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationPath* obj = (CAnnotationPath*)_obj;
    obj->renderSelf();
}
