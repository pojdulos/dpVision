#include "IMovementRenderer.h"
#include <qopengl.h>
#include "Movement.h"

void IMovementRenderer::renderKids(const CBaseObject* _obj)
{
	CMovement* obj = (CMovement*)_obj;
	obj->renderFrame();
}

