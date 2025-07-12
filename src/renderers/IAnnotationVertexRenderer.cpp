#include "IAnnotationVertexRenderer.h"
#include <qopengl.h>
#include "AnnotationVertex.h"

void IAnnotationVertexRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationVertex* obj = (CAnnotationVertex*)_obj;
    obj->renderSelf();
}
