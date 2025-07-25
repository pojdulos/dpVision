#include "IAnnotationSetOfFacesRenderer.h"
#include <qopengl.h>
#include "AnnotationSetOfFaces.h"

void IAnnotationSetOfFacesRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationSetOfFaces* obj = (CAnnotationSetOfFaces*)_obj;

	if (obj->size() > 1)
	{
		if (auto m_mesh = obj->getDest())
		{
			glPushMatrix();
			glPushAttrib(GL_ALL_ATTRIB_BITS);

			glDisable(GL_TEXTURE_2D);

			glPolygonMode(GL_FRONT, GL_FILL);
			glPolygonMode(GL_BACK, GL_FILL);
			glEnable(GL_COLOR_MATERIAL);
			glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

			if (obj->isChecked())
			{
				glLineWidth(5.0);
				glColor4ubv(obj->getSelColor().V());
			}
			else
			{
				glLineWidth(3.0);
				glColor4ubv(obj->getColor().V());
			}


			glBegin(GL_TRIANGLES);

			SetOfFIndices::iterator it;
			for (it = obj->begin(); it != obj->end(); it++)
			{
				//glNormal3fv( mesh->fnormals()[*it].toVector() );

				glVertex3fv(m_mesh->vertices()[m_mesh->faces()[*it].A()].toVector());
				glVertex3fv(m_mesh->vertices()[m_mesh->faces()[*it].B()].toVector());
				glVertex3fv(m_mesh->vertices()[m_mesh->faces()[*it].C()].toVector());
			}
			glEnd();

			glDisable(GL_COLOR_MATERIAL);

			glPopAttrib();
			glPopMatrix();
		}

	}
}