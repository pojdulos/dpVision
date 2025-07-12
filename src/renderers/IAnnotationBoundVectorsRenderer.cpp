#include "IAnnotationBoundVectorsRenderer.h"
#include <qopengl.h>
#include "AnnotationBoundVectors.h"

void IAnnotationBoundVectorsRenderer::renderSelf(const CBaseObject* _obj)
{
	CAnnotationBoundVectors* obj = (CAnnotationBoundVectors*)_obj;

	obj->renderSelf();
}

