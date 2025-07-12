#include "IAnnotationSetOfFacesRenderer.h"
#include <qopengl.h>
#include "AnnotationSetOfFaces.h"

void IAnnotationSetOfFacesRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationSetOfFaces* obj = (CAnnotationSetOfFaces*)_obj;
    obj->renderSelf();
}
