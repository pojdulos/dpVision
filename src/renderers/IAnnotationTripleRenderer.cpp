#include "IAnnotationTripleRenderer.h"
#include <qopengl.h>
#include "AnnotationTriple.h"

void IAnnotationTripleRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationTriple* obj = (CAnnotationTriple*)_obj;
    obj->renderSelf();
}
