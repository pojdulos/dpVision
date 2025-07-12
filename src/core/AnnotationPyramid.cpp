#include "Global.h"
#include "AnnotationPyramid.h"

#include "AP.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#include "../renderers/IAnnotationPyramidRenderer.h"

CAnnotationPyramid::CAnnotationPyramid(std::shared_ptr<CBaseObject> m) :CAnnotation(m)
{
	setLabel("Pyramid");
	renderer_ = std::make_shared<IAnnotationPyramidRenderer>();
}

CAnnotationPyramid::CAnnotationPyramid(CAnnotationPyramid& p) : CAnnotation(p)
{
	setLabel("Pyramid");
	m_top[0] = p.m_top[0];
	m_top[1] = p.m_top[1];
	m_top[2] = p.m_top[2];
	m_top[3] = p.m_top[3];
	m_bottom[0] = p.m_bottom[0];
	m_bottom[1] = p.m_bottom[1];
	m_bottom[2] = p.m_bottom[2];
	m_bottom[3] = p.m_bottom[3];
	renderer_ = std::make_shared<IAnnotationPyramidRenderer>();
}

std::wstring CAnnotationPyramid::getInfoRow()
{
	std::wstring info = L"Pyramid";

	return info;
}

void CAnnotationPyramid::renderSelf()
{
	glPushMatrix();
	glPushAttrib( GL_ALL_ATTRIB_BITS );

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	
	//glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);	


	if ( m_selected )
	{
		glColor4ubv(m_selcolor.V());
	}
	else
	{
		glColor4ubv(m_color.V());
	}

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);

	glBegin(GL_POLYGON);
		glNormal3dv( CVector3d(m_top[0], m_top[1]).crossProduct( CVector3d(m_top[0],m_bottom[0]) ).getNormalized().toVector() );
		glVertex3dv(m_top[0].toVector() );
		glVertex3dv(m_bottom[0].toVector());
		glVertex3dv(m_bottom[1].toVector());
		glVertex3dv(m_top[1].toVector());
	glEnd();

	glBegin(GL_POLYGON);
	glNormal3dv(CVector3d(m_top[1], m_top[2]).crossProduct(CVector3d(m_top[1], m_bottom[1])).getNormalized().toVector());
	glVertex3dv(m_top[1].toVector());
		glVertex3dv(m_bottom[1].toVector());
		glVertex3dv(m_bottom[2].toVector());
		glVertex3dv(m_top[2].toVector());
	glEnd();

	glBegin(GL_POLYGON);
	glNormal3dv(CVector3d(m_top[2], m_top[3]).crossProduct(CVector3d(m_top[2], m_bottom[2])).getNormalized().toVector());
	glVertex3dv(m_top[2].toVector());
		glVertex3dv(m_bottom[2].toVector());
		glVertex3dv(m_bottom[3].toVector());
		glVertex3dv(m_top[3].toVector());
	glEnd();

	glBegin(GL_POLYGON);
	glNormal3dv(CVector3d(m_top[3], m_top[0]).crossProduct(CVector3d(m_top[3], m_bottom[3])).getNormalized().toVector());
	glVertex3dv(m_top[3].toVector());
		glVertex3dv(m_bottom[3].toVector());
		glVertex3dv(m_bottom[0].toVector());
		glVertex3dv(m_top[0].toVector());
	glEnd();

	glLineWidth(5);

	glColor4b( 255, 255, 0, 0 );

	glBegin(GL_LINES);
		glVertex3dv(m_top[0].toVector());
		glVertex3dv(m_bottom[0].toVector());

		glVertex3dv(m_top[1].toVector());
		glVertex3dv(m_bottom[1].toVector());

		glVertex3dv(m_top[2].toVector());
		glVertex3dv(m_bottom[2].toVector());

		glVertex3dv(m_top[3].toVector());
		glVertex3dv(m_bottom[3].toVector());
	glEnd();

	glPopAttrib();
	glPopMatrix();
}
