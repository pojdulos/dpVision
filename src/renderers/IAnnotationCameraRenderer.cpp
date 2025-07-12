#include "IAnnotationCameraRenderer.h"
#include <qopengl.h>
#include "AnnotationCamera.h"

void IAnnotationCameraRenderer::renderSelf(const CBaseObject* _obj)
{
	CAnnotationCamera* obj = (CAnnotationCamera*)_obj;

	obj->renderSelf();
}

