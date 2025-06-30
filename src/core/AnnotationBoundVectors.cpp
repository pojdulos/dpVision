#include "Global.h"
#include "AnnotationBoundVectors.h"

#include "AP.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

void CAnnotationBoundVectors::applyTransformation(CTransform &prevT, CTransform &newT)
{
	for ( std::list<_BoundVector>::iterator ip = this->m_data.begin(); ip != this->m_data.end(); ip++ )
	{
		ip->first = newT.w2l(prevT.l2w(ip->first));
		ip->second = newT.w2l(prevT.l2w(ip->second));
	}
}

std::wstring CAnnotationBoundVectors::getInfoRow()
{
	std::wstring info = L"Size: " + std::to_wstring((long long) m_data.size()) + L" vectors.";

	return info;
}

void CAnnotationBoundVectors::renderSelf()
{
	if (this->m_data.size() > 0)
	{
		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		glDisable(GL_TEXTURE_2D);
		glEnable(GL_COLOR_MATERIAL);

		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

		std::list<_BoundVector>::iterator ip = this->m_data.begin();

		while (ip != this->m_data.end())
		{

			if (m_selected)
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
				glVertex3dv( ip->first.toVector() );
			glEnd();

			CVector3d v(ip->first, ip->second);
			CPoint3d p1 = ip->first;// -v;// *2;
			CPoint3d p2 = ip->first + v;// *2;

			glLineWidth(1);
			//glColor3f(1.0, 1.0, 0.0);
			glBegin(GL_LINES);
				//glVertex3dv( ip->first.toVector() );
				//glVertex3dv( ip->second.toVector() );
				glVertex3dv(p1.toVector());
				glVertex3dv(p2.toVector());
				glEnd();

			ip++;
		}

		glDisable(GL_COLOR_MATERIAL);
	
		glPopAttrib();
		glPopMatrix();
	}
}
