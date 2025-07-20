#include "IAnnotationBoxRenderer.h"
#include <qopengl.h>
#include "AnnotationBox.h"

void IAnnotationBoxRenderer::renderSelf(const CBaseObject* _obj)
{
	CAnnotationBox* obj = (CAnnotationBox*)_obj;

	glPushMatrix();
	glPushAttrib( GL_ALL_ATTRIB_BITS );

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	
	glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);	

	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_LINE);


	if ( obj->isChecked() )
	{
		glColor4ubv(obj->getSelColor().V());
	}
	else
	{
		glColor4ubv(obj->getColor().V());
	}

	glLineWidth(5);
	
	auto m_max = obj->getMax();
	auto m_min = obj->getMin();

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
	glVertex3f(m_min.X(), m_min.Y(), m_max.Z());

	glVertex3f(m_min.X(), m_min.Y(), m_min.Z());
	glVertex3f(m_min.X(), m_max.Y(), m_min.Z());
	glVertex3f(m_max.X(), m_max.Y(), m_min.Z());
	glVertex3f(m_max.X(), m_min.Y(), m_min.Z());

	glEnd();

	glPopAttrib();
	glPopMatrix();
}
