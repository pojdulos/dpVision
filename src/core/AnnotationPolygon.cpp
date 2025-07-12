#include "Global.h"
#include "AnnotationPolygon.h"

#include "AP.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#include "../renderers/IAnnotationPolygonRenderer.h"

CAnnotationPolygon::CAnnotationPolygon(std::shared_ptr<CBaseObject> m) :CAnnotation(m)
{
	setLabel("polygon");
	renderer_ = std::make_shared<IAnnotationPolygonRenderer>();
}

CAnnotationPolygon::CAnnotationPolygon(CAnnotationPolygon& p) : CAnnotation(p)
{
	setLabel("polygon");
	m_data = p.m_data;
	renderer_ = std::make_shared<IAnnotationPolygonRenderer>();
}

std::wstring CAnnotationPolygon::getInfoRow()
{
	std::wstring info = L"Size: " + std::to_wstring((long long)m_data.size());

	return info;
}

void CAnnotationPolygon::renderSelf()
{
	glPushMatrix();
	glPushAttrib( GL_ALL_ATTRIB_BITS );

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);	


	if ( m_selected )
	{
		glColor4ubv(m_selcolor.V());
	}
	else
	{
		glColor4ubv(m_color.V());
	}

	glLineWidth(5);
	
	glBegin(GL_POLYGON);
	for (std::list<CPoint3d>::iterator it = m_data.begin(); it != m_data.end(); it++)
	{
		glVertex3f( it->X(), it->Y(), it->Z() );
	}
	glEnd();

	glPopAttrib();
	glPopMatrix();
}
