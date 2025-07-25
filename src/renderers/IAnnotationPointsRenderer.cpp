#include "IAnnotationPointsRenderer.h"
#include <qopengl.h>
#include "AnnotationPoints.h"

void IAnnotationPointsRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationPoints* obj = (CAnnotationPoints*)_obj;
 
    if (obj->m_list.size() > 0)
	{
		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		glDisable(GL_TEXTURE_2D);
		glEnable(GL_COLOR_MATERIAL);

		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

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

		SetOfPoints::iterator ip = obj->m_list.begin();

		while (ip != obj->m_list.end())
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
