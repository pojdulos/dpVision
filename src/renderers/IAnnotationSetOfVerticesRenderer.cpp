#include "IAnnotationSetOfVerticesRenderer.h"
#include <qopengl.h>
#include "AnnotationSetOfVertices.h"

void IAnnotationSetOfVerticesRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationSetOfVertices* obj = (CAnnotationSetOfVertices*)_obj;

	obj->getDest();

	if (obj->size() > 0)
	{
		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		glDisable(GL_TEXTURE_2D);
		glEnable(GL_COLOR_MATERIAL);

		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

		glPointSize(obj->m_pointSize);

		glColor4ubv(obj->isSelected() ? obj->getSelColor().V() : obj->getColor().V());

		glEnable(GL_POINT_SMOOTH);
		glBegin(GL_POINTS);

		for (auto idx : *obj)
		{
			glVertex3fv(obj->m_cloud->vertices()[idx].toVector());
		}

		glEnd();

		glDisable(GL_COLOR_MATERIAL);

		glPopAttrib();
		glPopMatrix();
	}
}