#include "IAnnotationCylinderRenderer.h"
#include <qopengl.h>
#include "AnnotationCylinder.h"

#include <math.h>

void IAnnotationCylinderRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationCylinder* obj = (CAnnotationCylinder*)_obj;

    glPushMatrix();
	glPushAttrib( GL_ALL_ATTRIB_BITS );

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);	

	if ( obj->isChecked() )
	{
		glPointSize(9);
		glColor4ubv(obj->getSelColor().V());
	}
	else
	{
		glPointSize(9);
		glColor4ubv(obj->getColor().V());
	}

	glTranslatef( obj->m_point.X(), obj->m_point.Y(), obj->m_point.Z() );

	double step = 2.0 * PI / 360.0;
	double a = step * obj->m_start_angle;

	glBegin(GL_TRIANGLE_STRIP);

	for (int i = obj->m_start_angle; i <= obj->m_end_angle; i++ )
	{
		float x = cos(a) * obj->m_radius;
		float y = sin(a) * obj->m_radius;

		glVertex3f( x, y, -obj->m_halfLength );
		glVertex3f( x, y, obj->m_halfLength );

		a += step;
	}

	glEnd();

	/*
	GLUquadricObj *quadObj;

	//glNewList(CYLINDER, GL_COMPILE);
	quadObj = gluNewQuadric();
	gluQuadricDrawStyle(quadObj, GLU_FILL);
	gluQuadricNormals(quadObj, GLU_SMOOTH);
	gluCylinder(quadObj, m_radius, m_radius, m_halfLength, 36, 1);
	//glEndList();

	//glCallList(CYLINDER);
	*/

	glPopAttrib();
	glPopMatrix();
}
