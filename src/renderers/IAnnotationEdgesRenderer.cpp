#include "IAnnotationEdgesRenderer.h"
#include <qopengl.h>
#include "AnnotationEdges.h"

void IAnnotationEdgesRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationEdges* obj = (CAnnotationEdges*)_obj;
    obj->renderSelf();
}
