#include "Global.h"
#include "AnnotationPath.h"

#include "AP.h"

//#include <Windows.h>
#include <GL/gl.h>


CAnnotationPath::iterator CAnnotationPath::cyclicNext(CAnnotationPath::iterator current)
{
	CAnnotationPath::iterator next = current;

	next++;
	if (next == end()) next = begin();

	return next;
}

CAnnotationPath::iterator CAnnotationPath::cyclicPrev(CAnnotationPath::iterator current)
{
	CAnnotationPath::iterator prev = current;

	if (prev == begin()) prev = end();
	prev--;

	return CAnnotationPath::iterator();
}

std::wstring CAnnotationPath::getInfoRow()
{
	std::wstring info = L"Size: " + std::to_wstring((long long)size()) + L" points.";

	return info;
}

void CAnnotationPath::renderSelf()
{
	if (std::list<CPoint3d>::size() > 1)
	{
		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		glDisable(GL_TEXTURE_2D);

		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
		glLineWidth(1.0);
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

		if (m_selected)
		{
			glColor4ubv(m_selcolor.V());
		}
		else
		{
			glColor4ubv(m_color.V());
		}

		glBegin(GL_LINE_STRIP);

		ListOfPoints::iterator ip = begin();

		while (ip != end())
		{
			CVertex v2 = *ip;
			glVertex3d(v2.X(), v2.Y(), v2.Z());
			ip++;
		}

		glEnd();

/*
		glBegin(GL_LINES);

		ListOfPoints::iterator ip = begin();

		CVertex v1 = *ip;

		while (++ip != this->m_data.end())
		{
			CVertex v2 = *ip;

			glVertex3d( v1.X(), v1.Y(), v1.Z() );
			glVertex3d( v2.X(), v2.Y(), v2.Z() );

			v1 = v2;
		}

		glEnd();
*/
		glDisable(GL_COLOR_MATERIAL);
	
		glPopAttrib();
		glPopMatrix();
	}
}
