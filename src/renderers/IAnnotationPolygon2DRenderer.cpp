#include "IAnnotationPolygon2DRenderer.h"
#include <qopengl.h>
#include "AnnotationPolygon2D.h"
#include "Model3D.h"
#include "Mesh.h"
#include "../api/UI.h"

void IAnnotationPolygon2DRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationPolygon2D* obj = (CAnnotationPolygon2D*)_obj;

	if (auto mdl = std::dynamic_pointer_cast<CModel3D>(obj->getParentPtr())) {
		if (auto mesh = std::dynamic_pointer_cast<CMesh>(mdl->getChild())) {

			glPushMatrix();
			glPushAttrib(GL_ALL_ATTRIB_BITS);

			glDisable(GL_TEXTURE_2D);
			glEnable(GL_COLOR_MATERIAL);

			glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);


			if (obj->isChecked())
			{
				glColor4ubv(obj->getSelColor().V());
			}
			else
			{
				glColor4ubv(obj->getColor().V());
			}

			//glViewport(0, 0, 1920, 1080);
			//glMatrixMode(GL_PROJECTION);
			//glLoadIdentity();
			//glOrtho(0, 1920, 1080, 20, -1, 1);
			//glMatrixMode(GL_MODELVIEW);
			//glLoadIdentity();

			glLineWidth(5);

			glBegin(GL_POLYGON);
			for (auto pt2d : obj->m_pairs)
			{

				CPoint3d MousePt(pt2d.first, pt2d.second, 5);
				CPoint3d Pt;

				UI::CAMERA::convertWinToWorld(MousePt, Pt);
				Pt = mdl->getTransform().world2local(Pt);

				glVertex3dv(Pt.toVector());
			}
			glEnd();

			glPopAttrib();
			glPopMatrix();

		}
	}
}
