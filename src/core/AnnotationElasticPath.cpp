#include "Global.h"
#include "AnnotationElasticPath.h"

#include "AP.h"

//#include <Windows.h>
#include <GL/gl.h>

std::wstring CAnnotationElasticPath::getInfoRow()
{
	std::wstring info = L"Size: " + std::to_wstring((long long) m_data.size()) + L" points.";

	return info;
}

void CAnnotationElasticPath::renderSelf()
{
	if (this->m_data.size() > 1)
	{
		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		glDisable(GL_TEXTURE_2D);

		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
		glLineWidth(4.0);
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

		glBegin(GL_LINES);

		std::list<CPoint3f>::iterator ip = this->m_data.begin();

		CVertex v1 = *ip;

		while (++ip != this->m_data.end())
		{
			CVertex v2 = *ip;

			glVertex3d( v1.X(), v1.Y(), v1.Z() );
			glVertex3d( v2.X(), v2.Y(), v2.Z() );

			v1 = v2;
		}

		glEnd();
		
		glDisable(GL_COLOR_MATERIAL);
	
		glPopAttrib();
		glPopMatrix();
	}
}
