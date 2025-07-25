#include "IAnnotationBoundVectorsRenderer.h"
#include <qopengl.h>
#include "AnnotationBoundVectors.h"

void IAnnotationBoundVectorsRenderer::renderSelf(const CBaseObject* _obj)
{
	CAnnotationBoundVectors* obj = (CAnnotationBoundVectors*)_obj;

	if (obj->m_data.size() > 0)
	{
		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		glDisable(GL_TEXTURE_2D);
		glEnable(GL_COLOR_MATERIAL);

		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

		auto ip = obj->m_data.begin();

		while (ip != obj->m_data.end())
		{

			if (obj->isChecked())
			{
				glPointSize(3);
				glColor4ubv(obj->getSelColor().V());
			}
			else
			{
				glPointSize(3);
				glColor4ubv(obj->getColor().V());
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
