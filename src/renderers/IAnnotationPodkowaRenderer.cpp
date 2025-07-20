#include "IAnnotationPodkowaRenderer.h"
#include <qopengl.h>
#include "AnnotationPodkowa.h"

void IAnnotationPodkowaRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationPodkowa* obj = (CAnnotationPodkowa*)_obj;

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

	double left_a = step * obj->m_start_angle;
	CPoint3f leftP0( cos(left_a) * obj->m_radius, sin(left_a) * obj->m_radius, 0.0 );
	//CVector3f leftVn( leftP0 );


	CPoint3f leftP1 = leftP0 + CPoint3f( obj->m_arm_length * sin( left_a ), -obj->m_arm_length * cos( left_a ), 0.0 );


	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f( leftP0.X(), leftP0.Y(), -obj->m_halfLength );
	glVertex3f( leftP0.X(), leftP0.Y(),  obj->m_halfLength );

	glVertex3f( leftP1.X(), leftP1.Y(), -obj->m_halfLength);
	glVertex3f( leftP1.X(), leftP1.Y(),  obj->m_halfLength);
	glEnd();

	
	double right_a = step * obj->m_end_angle;
	CPoint3f rightP0(cos(right_a) * obj->m_radius, sin(right_a) * obj->m_radius, (double)obj->m_point.Z());
	//CVector3f rightVn( rightP0 );

	CPoint3f rightP1 = rightP0 + CPoint3f( -obj->m_arm_length * sin(left_a), -obj->m_arm_length * cos(left_a), 0.0);
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(rightP0.X(), rightP0.Y(), -obj->m_halfLength);
	glVertex3f(rightP0.X(), rightP0.Y(), obj->m_halfLength);

	glVertex3f(rightP1.X(), rightP1.Y(), -obj->m_halfLength);
	glVertex3f(rightP1.X(), rightP1.Y(), obj->m_halfLength);
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
