#include "Global.h"
#include "AnnotationBox.h"

#include "AP.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#include "../renderers/IAnnotationBoxRenderer.h"

CAnnotationBox::CAnnotationBox(std::shared_ptr<CBaseObject> m) :CAnnotation(m)
{
	setLabel("box");

	renderer_ = std::make_shared<IAnnotationBoxRenderer>();
}

CAnnotationBox::CAnnotationBox(CAnnotationBox& p) : CAnnotation(p)
{
	setLabel("box");
	//m_min = p.m_min;
	//m_max = p.m_max;

	renderer_ = std::make_shared<IAnnotationBoxRenderer>();
}

std::wstring CAnnotationBox::getInfoRow()
{
	std::wstring info = L"";// MinSize: " + std::to_wstring((long long)m_data.size());

	return info;
}

void CAnnotationBox::renderSelf()
{
	glPushMatrix();
	glPushAttrib( GL_ALL_ATTRIB_BITS );

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	
	glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);	

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);


	if ( m_selected )
	{
		glColor4ubv(m_selcolor.V());
	}
	else
	{
		glColor4ubv(m_color.V());
	}

	glLineWidth(5);
	
	glBegin(GL_QUADS);

	glVertex3f(m_max.X(), m_min.Y(), m_min.Z());
	glVertex3f(m_max.X(), m_max.Y(), m_min.Z());
	glVertex3f(m_max.X(), m_max.Y(), m_max.Z());
	glVertex3f(m_max.X(), m_min.Y(), m_max.Z());

	glVertex3f(m_min.X(), m_max.Y(), m_min.Z());
	glVertex3f(m_min.X(), m_min.Y(), m_min.Z());
	glVertex3f(m_min.X(), m_min.Y(), m_max.Z());
	glVertex3f(m_min.X(), m_max.Y(), m_max.Z());

	glVertex3f(m_max.X(), m_max.Y(), m_min.Z());
	glVertex3f(m_min.X(), m_max.Y(), m_min.Z());
	glVertex3f(m_min.X(), m_max.Y(), m_max.Z());
	glVertex3f(m_max.X(), m_max.Y(), m_max.Z());

	glVertex3f(m_max.X(), m_min.Y(), m_max.Z());
	glVertex3f(m_min.X(), m_min.Y(), m_max.Z());
	glVertex3f(m_min.X(), m_min.Y(), m_min.Z());
	glVertex3f(m_max.X(), m_min.Y(), m_min.Z());

	glVertex3f(m_max.X(), m_min.Y(), m_max.Z());
	glVertex3f(m_max.X(), m_max.Y(), m_max.Z());
	glVertex3f(m_min.X(), m_max.Y(), m_max.Z());
	glVertex3f( m_min.X(), m_min.Y(), m_max.Z());

	glVertex3f(m_min.X(), m_min.Y(), m_min.Z());
	glVertex3f(m_min.X(), m_max.Y(), m_min.Z());
	glVertex3f(m_max.X(), m_max.Y(), m_min.Z());
	glVertex3f(m_max.X(), m_min.Y(), m_min.Z());

	glEnd();

	glPopAttrib();
	glPopMatrix();
}
