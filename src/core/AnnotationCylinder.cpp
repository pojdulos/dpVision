#include "Global.h"
#include "AnnotationCylinder.h"

#include "AP.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h> 

#include "../renderers/IAnnotationCylinderRenderer.h"

CAnnotationCylinder::CAnnotationCylinder(std::shared_ptr<CBaseObject> m) :CAnnotation(m)
{
	setLabel("cylinder");
	m_point = CPoint3f(0, 0, 0);
	m_radius = 0;
	m_halfLength = 0;
	m_start_angle = 0.0;
	m_end_angle = 360.0;
	renderer_ = std::make_shared<IAnnotationCylinderRenderer>();
}

CAnnotationCylinder::CAnnotationCylinder(CAnnotationCylinder& t) :CAnnotation(t)
{
	setLabel("cylinder");
	m_point = t.m_point;
	m_radius = t.m_radius;
	m_halfLength = t.m_halfLength;
	m_start_angle = t.m_start_angle;
	m_end_angle = t.m_end_angle;
	renderer_ = std::make_shared<IAnnotationCylinderRenderer>();
}

std::wstring CAnnotationCylinder::getInfoRow()
{
	std::wstring info = L"Cylinder";
	
	return info;
}

#include <math.h>
void CAnnotationCylinder::renderSelf()
{
	glPushMatrix();
	glPushAttrib( GL_ALL_ATTRIB_BITS );

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);	

	if ( m_selected )
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
