#include "IAnnotationVPathRenderer.h"
#include <qopengl.h>
#include "AnnotationVPath.h"

void IAnnotationVPathRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationVPath* obj = (CAnnotationVPath*)_obj;

	if (auto m_parent = std::dynamic_pointer_cast<CObject>(obj->getParentPtr()))
	{
		if (obj->size() > 1)
		{
			if (auto mesh = std::dynamic_pointer_cast<CMesh>(m_parent->getChild()))
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
					glLineWidth(5.0);
					glColor4ubv(obj->getSelColor().V());
				}
				else
				{
					glLineWidth(3.0);
					glColor4ubv(obj->getColor().V());
				}

				glBegin(GL_LINES);

				ListOfVIndices::iterator ip = obj->begin();

				CVertex v1 = mesh->vertices()[*ip];

				while (++ip != obj->end())
				{
					CVertex v2 = mesh->vertices()[*ip];

					glVertex3fv(v1.toVector());
					glVertex3fv(v2.toVector());

					v1 = v2;
				}

				glEnd();

				glDisable(GL_COLOR_MATERIAL);

				glPopAttrib();
				glPopMatrix();
			}
		}
	}
}
