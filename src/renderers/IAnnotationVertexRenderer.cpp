#include "IAnnotationVertexRenderer.h"
#include <qopengl.h>
#include "AnnotationVertex.h"
#include "OrderedPointCloud.h"
#include "Mesh.h"

std::shared_ptr<CPointCloud> getDest(CAnnotationVertex* obj) {
	if (obj->m_cloud) return obj->m_cloud;

	if (auto shp = std::dynamic_pointer_cast<CObject>(obj->getParentPtr())) {
		if (auto parent_cloud = std::dynamic_pointer_cast<CPointCloud>(shp)) return parent_cloud;
		else if (auto parent_cloud = std::dynamic_pointer_cast<COrderedPointCloud>(shp)) return parent_cloud;
		else if (auto parent_cloud = std::dynamic_pointer_cast<CMesh>(shp)) return parent_cloud;
		else {
			if (auto child_cloud = std::dynamic_pointer_cast<CPointCloud>(shp->getChild())) return child_cloud;
			else if (auto child_cloud = std::dynamic_pointer_cast<COrderedPointCloud>(shp->getChild())) return child_cloud;
			else if (auto child_cloud = std::dynamic_pointer_cast<CMesh>(shp->getChild())) return child_cloud;
		}
	}
	return nullptr;
}

void IAnnotationVertexRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationVertex* obj = (CAnnotationVertex*)_obj;

	if (auto v = obj->getVertex()) {
		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		glDisable(GL_TEXTURE_2D);
		glEnable(GL_COLOR_MATERIAL);

		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

		if (obj->isChecked())
		{
			glPointSize(9);
			glColor4ubv(obj->getSelColor().V());
		}
		else
		{
			glPointSize(9);
			glColor4ubv(obj->getColor().V());
		}

		glTranslatef(0.0, 0.0, 0.0);

		glEnable(GL_POINT_SMOOTH);
		glBegin(GL_POINTS);
		glVertex3fv(v->toVector());
		glEnd();

		glPopAttrib();
		glPopMatrix();
	}
}
