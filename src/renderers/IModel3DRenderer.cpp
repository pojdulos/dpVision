#include "IModel3DRenderer.h"
#include <qopengl.h>
#include "../core/Model3D.h"
#include "../core/Annotation.h"
#include "../core/BoundingBox.h"

#include "../api/UI.h"

void IModel3DRenderer::renderBoundingBox(CModel3D* obj)
{
	QVector<std::shared_ptr<CBaseObject>> objts = UI::DOCK::WORKSPACE::getSelectedObjects();

	CBoundingBox::Style style = objts.contains(obj->shared_from_this()) ? CBoundingBox::Style::Unlocked : CBoundingBox::Style::NotSelected;

	obj->draw(style, obj->isChecked());
}

void IModel3DRenderer::renderTransform(const CBaseObject* _obj)
{
	CModel3D* obj = (CModel3D*)_obj;
	if (obj->getSelfVisibility()) obj->transform().render();
	if (obj->bDrawBB) renderBoundingBox(obj);
}


void IModel3DRenderer::renderSelf(const CBaseObject* _obj)
{
	//if (bDrawBB) renderAxes();

	//if (m_visible != Visibility::HIDE_ALL)
	//{
	//	renderKids();
	//}
}


void IModel3DRenderer::renderAxes()
{
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	float buf[4];
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, CRGBA(_DEF_AMBIENT).fV(buf));
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, CRGBA(_DEF_DIFFUSE).fV(buf));
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, CRGBA(_DEF_SPECULAR).fV(buf));
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, CRGBA(_DEF_EMISSION).fV(buf));
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, _DEF_SHININESS);

	glLineWidth(2.0);

	int L = 5;

	glColor4f(1.0f, 0.0f, 1.0f, 1.0f);

	glBegin(GL_LINES);

	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertex3f(-L, 0, 0);
	glVertex3f(L, 0, 0);

	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex3f(0, -L, 0);
	glVertex3f(0, L, 0);

	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	glVertex3f(0, 0, -L);
	glVertex3f(0, 0, L);

	glEnd();

	//glColor3f(0.5f, 0.5f, 0.5f);
	//glEnable(GL_POINT_SMOOTH);
	//glPointSize(5);
	//glBegin(GL_POINTS);
	//glVertex3f(0, 0, 0);
	//glEnd();

	glDisable(GL_COLOR_MATERIAL);

	glPopAttrib();
}



void drawBox(CPoint3f min, CPoint3f max, bool dashed = false)
{
	if (dashed)
	{
		glLineStipple(3, 0x00FF);
		glEnable(GL_LINE_STIPPLE);
	}

	glPushAttrib(GL_ENABLE_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBegin(GL_QUAD_STRIP);
	glVertex3f(max.X(), min.Y(), min.Z());
	glVertex3f(max.X(), min.Y(), max.Z());

	glVertex3f(min.X(), min.Y(), min.Z());
	glVertex3f(min.X(), min.Y(), max.Z());

	glVertex3f(min.X(), max.Y(), min.Z());
	glVertex3f(min.X(), max.Y(), max.Z());

	glVertex3f(max.X(), max.Y(), min.Z());
	glVertex3f(max.X(), max.Y(), max.Z());

	glVertex3f(max.X(), min.Y(), min.Z());
	glVertex3f(max.X(), min.Y(), max.Z());
	glEnd();

	glPopAttrib();
}

//void CModel3D::renderBoundingBox()
//{
//	CBoundingBox::Style style;
//
//	if (AP::WORKSPACE::getCurrentModelId() != m_Id)
//		style = CBoundingBox::Style::NotSelected;
//	else
//		if (isLocked())
//			style = CBoundingBox::Style::Locked;
//		else
//			style = CBoundingBox::Style::Unlocked;
//
//	if (AP::WORKSPACE::SELECTION::isModelSelected(m_Id))
//	{
//		CBoundingBox::draw(style, true);
//	}
//	else
//	{
//		CBoundingBox::draw(style, false);
//	}
//}

//void CModel3D::renderBoundingBox()
//{
//	glPushAttrib(GL_ALL_ATTRIB_BITS);
//
//	glDisable(GL_TEXTURE_2D);
//
//	glEnable(GL_COLOR_MATERIAL);
//	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
//
//	glLineWidth(1.0);
//	if ( AP::WORKSPACE::getCurrentModelId() == m_Id )
//		if (isLocked())
//			glColor3f(1.0f, 0.0f, 0.0f);
//		else
//			glColor3f(0.0f, 0.5f, 0.0f);
//	else
//		glColor3f(0.2f, 0.2f, 0.2f);
//
//	drawBox(m_min, m_max);
//
//	if (AP::WORKSPACE::SELECTION::isModelSelected(m_Id))
//	{
//		glLineWidth(3.0);
//		glColor4f(0.5f, 0.5f, 0.0f, 0.5f);
//		drawBox(m_min, m_max, true);
//	}
//
//	glDisable(GL_COLOR_MATERIAL);
//
//	glPopAttrib();
//}
