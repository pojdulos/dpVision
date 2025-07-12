// src/renderers/IBaseObjectRenderer.cpp

#include "IBaseObjectRenderer.h"
#include <qopengl.h>
#include "../core/BaseObject.h"

void IBaseObjectRenderer::render(const CBaseObject* obj)
{
	glPushMatrix();
	renderTransform(obj);
	if (obj->getSelfVisibility()) renderSelf(obj);
	if (obj->getKidsVisibility()) renderKids(obj);
	glPopMatrix();
}
