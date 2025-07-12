#include "IAnnotationBezierRenderer.h"
#include <qopengl.h>
#include "AnnotationBezier.h"

void IAnnotationBezierRenderer::renderSelf(const CBaseObject* _obj)
{
	CAnnotationBezier* obj = (CAnnotationBezier*)_obj;

	obj->renderSelf();
}

