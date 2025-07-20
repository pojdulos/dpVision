#include "IAnnotationPyramidRenderer.h"
#include <qopengl.h>
#include "AnnotationPyramid.h"

void IAnnotationPyramidRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationPyramid* obj = (CAnnotationPyramid*)_obj;

    glPushMatrix();
	glPushAttrib( GL_ALL_ATTRIB_BITS );

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	
	//glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);	


	if ( obj->isChecked() )
	{
		glColor4ubv(obj->getSelColor().V());
	}
	else
	{
		glColor4ubv(obj->getColor().V());
	}

    auto m_top = obj->m_top;
    auto m_bottom = obj->m_bottom;

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
