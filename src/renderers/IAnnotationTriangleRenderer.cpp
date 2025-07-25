#include "IAnnotationTriangleRenderer.h"
#include <qopengl.h>
#include "AnnotationTriangle.h"

void IAnnotationTriangleRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationTriangle* obj = (CAnnotationTriangle*)_obj;

    glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glDisable(GL_TEXTURE_2D);
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

	//glLineWidth(5);

	glBegin(GL_TRIANGLES);
		glVertex3dv(obj->m_pA.toVector());
		glVertex3dv(obj->m_pB.toVector());
		glVertex3dv(obj->m_pC.toVector());
	glEnd();


	glPointSize(5);

	if (obj->isChecked())
	{
		glColor4ubv(obj->getSelColor().V());
	}
	else
	{
		glColor4ubv(obj->getColor().V());
	}

	glEnable(GL_POINT_SMOOTH);
	glBegin(GL_POINTS);
		glVertex3dv(obj->m_pA.toVector());
		glVertex3dv(obj->m_pB.toVector());
		glVertex3dv(obj->m_pC.toVector());
	glEnd();


	glPopAttrib();
	glPopMatrix();
}
