#include "IAnnotationEdgesRenderer.h"
#include <qopengl.h>
#include "AnnotationEdges.h"

void IAnnotationEdgesRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationEdges* obj = (CAnnotationEdges*)_obj;

    if (obj->m_list.size() > 1)
	{
		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		glDisable(GL_TEXTURE_2D);

		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

		if (obj->isChecked())
		{
			glLineWidth(1.0);
			glColor4ubv(obj->getSelColor().V() );
		}
		else
		{
			glLineWidth(1.0);
			glColor4ubv(obj->getColor().V() );
		}

		glBegin(GL_LINES);

		std::list<CAnnotationEdge>::iterator it;
		for ( it = obj->m_list.begin(); it != obj->m_list.end(); it++ )
		{
			glVertex3fv(it->first.toVector());
			glVertex3fv(it->second.toVector());
			//CVertex( it->first  ).render();
			//CVertex( it->second ).render();
			//glVertex3fv( it->first.toVector() );
			//glVertex3fv( it->second.toVector() );
		}
		glEnd();

		glDisable(GL_COLOR_MATERIAL);
	
		glPopAttrib();
		glPopMatrix();
	}
}
