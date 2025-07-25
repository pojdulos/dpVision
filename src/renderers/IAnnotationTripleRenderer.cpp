#include "IAnnotationTripleRenderer.h"
#include <qopengl.h>
#include "AnnotationTriple.h"

void IAnnotationTripleRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationTriple* obj = (CAnnotationTriple*)_obj;

	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	drawAxes(obj);

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
		glVertex3dv(obj->realPos(obj->m_front).toVector());
		glVertex3dv(obj->realPos(obj->m_left).toVector());
		glVertex3dv(obj->realPos(obj->m_right).toVector());
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
	glVertex3dv(obj->realPos(obj->m_front).toVector());
	glVertex3dv(obj->realPos(obj->m_left).toVector());
	glVertex3dv(obj->realPos(obj->m_right).toVector());
	glEnd();


	//if ( 0.0 != m_direction.length() )
	//{
	//	glLineWidth(1);
	//	glColor3f(1.0, 1.0, 0.0);
	//	glBegin(GL_LINES);
	//	glVertex3f(-5*this->m_direction.X(), -5*this->m_direction.Y(), -5*this->m_direction.Z());
	//	glVertex3f(0, 0, 0);
	//	glEnd();
	//}

	glPopAttrib();
	glPopMatrix();
}


void IAnnotationTripleRenderer::drawAxes(CAnnotationTriple* obj)
{
	glLineWidth(3);
	glBegin(GL_LINES);
	glColor3f(0.2f, 0.2f, 0.8f);
	glVertex3dv(obj->realPos(CPoint3d(0, 0, 0)).toVector()); glVertex3dv(obj->realPos(CPoint3d(10,  0, 0)).toVector());
	glColor3f(0.2f, 0.8f, 0.2f);
	glVertex3dv(obj->realPos(CPoint3d(0, 0, 0)).toVector()); glVertex3dv(obj->realPos(CPoint3d( 0, 10, 0)).toVector());
	glColor3f(0.8f, 0.2f, 0.2f);
	glVertex3dv(obj->realPos(CPoint3d(0, 0, 0)).toVector()); glVertex3dv(obj->realPos(CPoint3d( 0, 0, 10)).toVector());
	glEnd();
}
