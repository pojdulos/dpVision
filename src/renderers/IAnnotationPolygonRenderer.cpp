#include "IAnnotationPolygonRenderer.h"
#include <qopengl.h>
#include "AnnotationPolygon.h"

void IAnnotationPolygonRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationPolygon* obj = (CAnnotationPolygon*)_obj;
 
    glPushMatrix();
	glPushAttrib( GL_ALL_ATTRIB_BITS );

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);
	
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);	


	if ( obj->isChecked() )
	{
		glColor4ubv(obj->getSelColor().V());
	}
	else
	{
		glColor4ubv(obj->getColor().V());
	}

	glLineWidth(5);
	
	glBegin(GL_POLYGON);
	for (std::list<CPoint3d>::iterator it = obj->m_data.begin(); it != obj->m_data.end(); it++)
	{
		glVertex3f( it->X(), it->Y(), it->Z() );
	}
	glEnd();

	glPopAttrib();
	glPopMatrix();
}
