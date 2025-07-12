#include "IAnnotationBoxRenderer.h"
#include <qopengl.h>
#include "AnnotationBox.h"

void IAnnotationBoxRenderer::renderSelf(const CBaseObject* _obj)
{
	CAnnotationBox* obj = (CAnnotationBox*)_obj;

	obj->renderSelf();
}

