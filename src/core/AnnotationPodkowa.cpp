#include "Global.h"
#include "AnnotationPodkowa.h"

#include "AP.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h> 
#include <math.h>

#include "../renderers/IAnnotationPodkowaRenderer.h"

CAnnotationPodkowa::CAnnotationPodkowa(std::shared_ptr<CBaseObject> m) :CAnnotationCylinder(m)
{
	setLabel("podkowa");
	m_arm_length = 50.0;
	renderer_ = std::make_shared<IAnnotationPodkowaRenderer>();
}

CAnnotationPodkowa::CAnnotationPodkowa(CAnnotationPodkowa& t) :CAnnotationCylinder(t)
{
	setLabel("podkowa");
	m_arm_length = t.m_arm_length;
	renderer_ = std::make_shared<IAnnotationPodkowaRenderer>();
}

std::wstring CAnnotationPodkowa::getInfoRow()
{
	std::wstring info = L"Podkowa. R="+ std::to_wstring(m_radius) + L", a=" + std::to_wstring(m_start_angle) + L", L=" + std::to_wstring(m_arm_length);
	
	return info;
}


// std::list<CPoint3f[3]> CAnnotationPodkowa::getFaces()
// {

// 	return std::list<CPoint3f[3]>();
// }


void CAnnotationPodkowa::renderSelf()
{
	glPushMatrix();
	glPushAttrib( GL_ALL_ATTRIB_BITS );

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);	

	if ( m_checked )
	{
		glPointSize(9);
		glColor4ubv(m_selcolor.V());
	}
	else
	{
		glPointSize(9);
		glColor4ubv(m_color.V());
	}

	glTranslatef( m_point.X(), m_point.Y(), m_point.Z() );

	double step = 2.0 * PI / 360.0;
	double a = step * m_start_angle;

	glBegin(GL_TRIANGLE_STRIP);

	for (int i = m_start_angle; i <= m_end_angle; i++ )
	{
		float x = cos(a) * m_radius;
		float y = sin(a) * m_radius;

		glVertex3f( x, y, -m_halfLength );
		glVertex3f( x, y, m_halfLength );

		a += step;
	}
	glEnd();

	double left_a = step * m_start_angle;
	CPoint3f leftP0( cos(left_a) * m_radius, sin(left_a) * m_radius, 0.0 );
	//CVector3f leftVn( leftP0 );


	CPoint3f leftP1 = leftP0 + CPoint3f( m_arm_length * sin( left_a ), -m_arm_length * cos( left_a ), 0.0 );


	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f( leftP0.X(), leftP0.Y(), -m_halfLength );
	glVertex3f( leftP0.X(), leftP0.Y(),  m_halfLength );

	glVertex3f( leftP1.X(), leftP1.Y(), -m_halfLength);
	glVertex3f( leftP1.X(), leftP1.Y(),  m_halfLength);
	glEnd();

	
	double right_a = step * m_end_angle;
	CPoint3f rightP0(cos(right_a) * m_radius, sin(right_a) * m_radius, (double)m_point.Z());
	//CVector3f rightVn( rightP0 );

	CPoint3f rightP1 = rightP0 + CPoint3f( -m_arm_length * sin(left_a), -m_arm_length * cos(left_a), 0.0);
	glBegin(GL_TRIANGLE_STRIP);
	glVertex3f(rightP0.X(), rightP0.Y(), -m_halfLength);
	glVertex3f(rightP0.X(), rightP0.Y(), m_halfLength);

	glVertex3f(rightP1.X(), rightP1.Y(), -m_halfLength);
	glVertex3f(rightP1.X(), rightP1.Y(), m_halfLength);
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
