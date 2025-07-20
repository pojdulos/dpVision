#include "IAnnotationPathRenderer.h"
#include <qopengl.h>
#include "AnnotationPath.h"

void IAnnotationPathRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationPath* obj = (CAnnotationPath*)_obj;

    if (obj->ListOfPoints::size() > 1)
	{
		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		glDisable(GL_TEXTURE_2D);

		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
		glLineWidth(1.0);
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

		if (obj->isChecked())
		{
			glColor4ubv(obj->getSelColor().V());
		}
		else
		{
			glColor4ubv(obj->getColor().V());
		}

		glBegin(GL_LINE_STRIP);

		ListOfPoints::iterator ip = obj->begin();

		while (ip != obj->end())
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

		while (++ip != this->m_pairs.end())
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
