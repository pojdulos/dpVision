#include "IObjectRenderer.h"
#include <qopengl.h>
#include "../core/Object.h"
#include "../core/Annotation.h"
#include "../core/BoundingBox.h"

void IObjectRenderer::renderBoundingBox(CObject* obj)
{
	CBoundingBox::Style style = obj->isChecked()
		? CBoundingBox::Style::Unlocked
		: CBoundingBox::Style::NotSelected;

	obj->draw(style, obj->isChecked());
}

void IObjectRenderer::renderTransform(const CBaseObject* _obj)
{
	CObject* obj = (CObject*)_obj;
	if (obj->bDrawBB) renderBoundingBox(obj);
}


void IObjectRenderer::renderKids(const CBaseObject* _obj) {
	CObject* obj = (CObject*)_obj;

	for (int id : obj->orderedChildIds())
	{
		std::shared_ptr<CBaseObject> child = obj->getChild(id);
		if (child == nullptr)
		{
			continue;
		}

		switch (child->type())
		{
		case CObject::MODEL:
			child->render();
			break;
		default:
			glLoadName(child->id());
			glPushName(child->id());

			child->render();

			glPopName();
			glLoadName(0);
			break;
		}
	}

	for (int id : obj->orderedAnnotationIds())
	{
		if (CAnnotation* annotation = obj->annotation(id))
		{
			annotation->render();
		}
	}
}
