#include "IAnnotationSphereRenderer.h"
#include <qopengl.h>
#include "AnnotationSphere.h"

void IAnnotationSphereRenderer::renderSelf(const CBaseObject* _obj)
{
    CAnnotationSphere* obj = (CAnnotationSphere*)_obj;

    glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	//drawAxes();

	if (obj->isChecked())
	{
		glColor4ubv(obj->getSelColor().V());
	}
	else
	{
		glColor4ubv(obj->getColor().V());
	}

	drawSphere(obj, obj->m_lats, obj->m_longs);

	glPopAttrib();
	glPopMatrix();
}

void IAnnotationSphereRenderer::drawSphere(CAnnotationSphere *obj, int lats, int longs) {
    auto m_c = obj->center();
    auto m_r = obj->radius();

	glTranslatef(m_c.x, m_c.y, m_c.z);
	for (int i = 0; i <= lats; i++) {
		double lat0 = PI * (-0.5 + (double)(i - 1) / lats);
		double z0 = sin(lat0);
		double zr0 = cos(lat0);

		double lat1 = PI * (-0.5 + (double)i / lats);
		double z1 = sin(lat1);
		double zr1 = cos(lat1);

		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j <= longs; j++) {
			double lng = 2 * PI * (double)(j - 1) / longs;
			double x = cos(lng);
			double y = sin(lng);

			glNormal3f(x * zr0, y * zr0, z0);
			glVertex3f(m_r * x * zr0, m_r * y * zr0, m_r * z0);
			glNormal3f(x * zr1, y * zr1, z1);
			glVertex3f(m_r * x * zr1, m_r * y * zr1, m_r * z1);
		}
		glEnd();
	}
}

