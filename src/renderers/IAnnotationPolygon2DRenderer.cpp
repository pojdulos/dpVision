#include "IAnnotationPolygon2DRenderer.h"
#include <qopengl.h>
#include "AnnotationPolygon2D.h"

void IAnnotationPolygon2DRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationPolygon2D* obj = (CAnnotationPolygon2D*)_obj;
    obj->renderSelf();
}
