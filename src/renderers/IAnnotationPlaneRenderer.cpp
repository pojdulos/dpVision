#include "IAnnotationPlaneRenderer.h"
#include <qopengl.h>
#include "AnnotationPlane.h"

void IAnnotationPlaneRenderer::renderSelf(const CBaseObject* _obj)
{
	CAnnotationPlane* obj = (CAnnotationPlane*)_obj;

	auto& normal = obj->m_normal;

	if (normal.length())
	{
		// wzór wyjściowy: n.x*v.x + n.y*v.y +n.z*v.z = 0
		// v.x = ( n.y*v.y + n.z*v.z ) / -n.x
		// v.y = ( n.x*v.x + n.z*v.z ) / -n.y
		// v.z = ( n.x*v.x + n.y*v.y ) / -n.z

		// przyjmuję wstępnie wartości v.x, v.y oraz v.z = 1.0 dla uproszczenia obliczeń
		double x = 1.0;
		double y = 1.0;
		double z = 1.0;


		// mianownik nie mo�e by� zerowy
		if (normal.Z() != 0.0) {
			z = (normal.X() + normal.Y()) / -normal.Z();
		}
		else if (normal.Y() != 0.0) {
			y = (normal.X() + normal.Z()) / -normal.Y();
		}
		else {
			x = (normal.Y() + normal.Z()) / -normal.X();
		}

		auto size = obj->getSize();

		CVector3d v1(CVector3d(x, y, z).getNormalized() * size);			// mamy wektor kierunkowy do pierwszego naro�nika, kt�ry mno�ymy od razu przez m_size
		CVector3d v2(v1.crossProduct(normal).getNormalized() * size);	// wektor prostopad�y jednocze�nie do normalnego i do v1 wyznaczy drugi naro�nik
		CVector3d v3(-v1);													// wektor przeciwny do v1 wyznacza trzeci naro�nik,
		CVector3d v4(-v2);													// a wektor przeciwny do v2 - czwarty

		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		glDisable(GL_TEXTURE_2D);
		glEnable(GL_COLOR_MATERIAL);

		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

		if (obj->isSelected())
		{
			glPointSize(9);
			glColor4ubv(obj->getSelColor().V());
		}
		else
		{
			glPointSize(9);
			glColor4ubv(obj->getColor().V());
		}

		glTranslatef(obj->m_center.X(), obj->m_center.Y(), obj->m_center.Z());

		//glBegin(GL_TRIANGLES);
		//glVertex3f(v1.X(), v1.Y(), v1.Z());
		//glVertex3f(v2.X(), v2.Y(), v2.Z());
		//glVertex3f(v3.X(), v3.Y(), v3.Z());

		//glVertex3f(v4.X(), v4.Y(), v4.Z());
		//glVertex3f(v1.X(), v1.Y(), v1.Z());
		//glVertex3f(v3.X(), v3.Y(), v3.Z());
		//glEnd();
		glBegin(GL_QUADS);
		glVertex3f(v1.X(), v1.Y(), v1.Z());
		glVertex3f(v2.X(), v2.Y(), v2.Z());
		glVertex3f(v3.X(), v3.Y(), v3.Z());
		glVertex3f(v4.X(), v4.Y(), v4.Z());
		glEnd();

		if (0.0 != normal.length())
		{
			glLineWidth(1);
			glColor3f(1.0, 1.0, 0.0);
			glBegin(GL_LINES);
			glVertex3f(5 * normal.X(), 5 * normal.Y(), 5 * normal.Z());
			glVertex3f(0, 0, 0);
			glEnd();
		}

		glPopAttrib();
		glPopMatrix();
	}
}

