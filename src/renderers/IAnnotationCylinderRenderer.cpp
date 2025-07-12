#include "IAnnotationCylinderRenderer.h"
#include <qopengl.h>
#include "AnnotationCylinder.h"

void IAnnotationCylinderRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationCylinder* obj = (CAnnotationCylinder*)_obj;
    obj->renderSelf();
}
