#include "Global.h"
#include "AnnotationPoints.h"

#include "AP.h"

//#include <Windows.h>
#include <GL/gl.h>

#include "../renderers/IAnnotationPointsRenderer.h"

CAnnotationPoints::CAnnotationPoints(std::shared_ptr<CBaseObject> m) :CAnnotation(m)
{
	setLabel("points");
	m_list.clear();
	renderer_ = std::make_shared<IAnnotationPointsRenderer>();
}

void CAnnotationPoints::applyTransformation(CTransform &prevT, CTransform &newT)
{
	for (SetOfPoints::iterator ip = this->m_list.begin(); ip != this->m_list.end(); ip++)
	{
		*ip = newT.w2l(prevT.l2w(*ip));
	}
}

std::wstring CAnnotationPoints::getInfoRow()
{
	std::wstring info = L"Size: " + std::to_wstring((long long) m_list.size()) + L" points.";

	return info;
}

void CAnnotationPoints::renderSelf()
{
	if (this->m_list.size() > 0)
	{
		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		glDisable(GL_TEXTURE_2D);
		glEnable(GL_COLOR_MATERIAL);

		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

		if (m_checked)
		{
			glPointSize(3);
			glColor4ubv(m_selcolor.V());
		}
		else
		{
			glPointSize(3);
			glColor4ubv(m_color.V());
		}

		glEnable(GL_POINT_SMOOTH);
		glBegin(GL_POINTS);

		SetOfPoints::iterator ip = this->m_list.begin();

		while (ip != this->m_list.end())
		{
			glVertex3d(ip->X(), ip->Y(), ip->Z());
			ip++;
		}

		glEnd();

		glDisable(GL_COLOR_MATERIAL);
	
		glPopAttrib();
		glPopMatrix();
	}
}
