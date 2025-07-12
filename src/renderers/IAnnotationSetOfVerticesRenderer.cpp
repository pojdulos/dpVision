#include "IAnnotationSetOfVerticesRenderer.h"
#include <qopengl.h>
#include "AnnotationSetOfVertices.h"

void IAnnotationSetOfVerticesRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationSetOfVertices* obj = (CAnnotationSetOfVertices*)_obj;
    obj->renderSelf();
}
