#include "IAnnotationRenderer.h"
#include <qopengl.h>
#include "Annotation.h"

void IAnnotationRenderer::renderTransform(const CBaseObject* _obj)
{
	CAnnotation* obj = (CAnnotation*)_obj;

	if (obj->getSelfVisibility() || obj->getKidsVisibility()) obj->transform().render();
}

void IAnnotationRenderer::renderKids(const CBaseObject* _obj) {
	CAnnotation* obj = (CAnnotation*)_obj;

	for (const auto& it : obj->annotations())
	{
		it.second->render();
	}
}
