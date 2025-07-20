#include "IObjectRenderer.h"
#include <qopengl.h>
#include "../core/Object.h"
#include "../core/Annotation.h"
#include "../core/BoundingBox.h"
#include "../api/UI.h"

void IObjectRenderer::renderBoundingBox(CObject* obj)
{
	QVector<std::shared_ptr<CBaseObject>> objts = UI::DOCK::WORKSPACE::getSelectedObjects();

	CBoundingBox::Style style = objts.contains(obj->shared_from_this()) ? CBoundingBox::Style::Unlocked : CBoundingBox::Style::NotSelected;

	obj->draw(style, obj->isChecked());
}

void IObjectRenderer::renderTransform(const CBaseObject* _obj)
{
	CObject* obj = (CObject*)_obj;
	if (obj->bDrawBB) renderBoundingBox(obj);
}


void IObjectRenderer::renderKids(const CBaseObject* _obj) {
	CObject* obj = (CObject*)_obj;

	for (const auto& itd : obj->children())
	{
		switch (itd.second->type())
		{
		case CObject::MODEL:
			itd.second->render();
			break;
		default:
			glLoadName(itd.second->id());
			glPushName(itd.second->id());

			itd.second->render();

			glPopName();
			glLoadName(0);
			break;
		}
	}

	for (const auto& it : obj->annotations())
	{
		it.second->render();
	}
}
