#include "Global.h"
#include "AnnotationPlane.h"

#include "AP.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

void CAnnotationPlane::applyTransformation(CTransform &prevT, CTransform &newT)
{
	CPoint3d p0(m_center);
	CPoint3d p1(p0 + CVector3d(m_normal));

	p0 = newT.w2l(prevT.l2w(p0));
	m_center = p0;

	if (m_normal.length())
	{
		p1 = newT.w2l(prevT.l2w(p1));

		m_normal = CVector3d(p0, p1).getNormalized();
	}
}


std::wstring CAnnotationPlane::getInfoRow()
{
	std::wstring info = L"Point: [ X=" + std::to_wstring((long double)m_center.X()) + L", Y=" + std::to_wstring((long double)m_center.Y()) + L", Z=" + std::to_wstring((long double)m_center.Z()) + L" ]";
	info.append( L", normal: [ nX=" + std::to_wstring((long double)m_normal.X()) + L", nY=" + std::to_wstring((long double)m_normal.Y()) + L", nZ=" + std::to_wstring((long double)m_normal.Z()) + L" ]" );

	return info;
}

void CAnnotationPlane::renderSelf()
{
	if (m_normal.length())
	{
		// wz�r wyj�ciowy: n.x*v.x + n.y*v.y +n.z*v.z = 0
		// v.x = ( n.y*v.y + n.z*v.z ) / -n.x
		// v.y = ( n.x*v.x + n.z*v.z ) / -n.y
		// v.z = ( n.x*v.x + n.y*v.y ) / -n.z

		// przyjmuj� wst�pnie warto�ci v.x, v.y oraz v.z = 1.0 dla uproszczenia oblicze�
		double x = 1.0;
		double y = 1.0;
		double z = 1.0;

		// mianownik nie mo�e by� zerowy
		if (m_normal.Z() != 0.0) {
			z = (m_normal.X() + m_normal.Y()) / -m_normal.Z();
		}
		else if (m_normal.Y() != 0.0) {
			y = (m_normal.X() + m_normal.Z()) / -m_normal.Y();
		}
		else {
			x = (m_normal.Y() + m_normal.Z()) / -m_normal.X();
		}

		
		CVector3d v1(CVector3d(x, y, z).getNormalized() * m_size);			// mamy wektor kierunkowy do pierwszego naro�nika, kt�ry mno�ymy od razu przez m_size
		CVector3d v2(v1.crossProduct(m_normal).getNormalized() * m_size);	// wektor prostopad�y jednocze�nie do normalnego i do v1 wyznaczy drugi naro�nik
		CVector3d v3(-v1);													// wektor przeciwny do v1 wyznacza trzeci naro�nik,
		CVector3d v4(-v2);													// a wektor przeciwny do v2 - czwarty

		glPushMatrix();
		glPushAttrib(GL_ALL_ATTRIB_BITS);

		glDisable(GL_TEXTURE_2D);
		glEnable(GL_COLOR_MATERIAL);

		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

		if (m_selected)
		{
			glPointSize(9);
			glColor4ubv(m_selcolor.V());
		}
		else
		{
			glPointSize(9);
			glColor4ubv(m_color.V());
		}

		glTranslatef(m_center.X(), m_center.Y(), m_center.Z());

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

		if (0.0 != m_normal.length())
		{
			glLineWidth(1);
			glColor3f(1.0, 1.0, 0.0);
			glBegin(GL_LINES);
			glVertex3f(5 * this->m_normal.X(), 5 * this->m_normal.Y(), 5 * this->m_normal.Z());
			glVertex3f(0, 0, 0);
			glEnd();
		}

		glPopAttrib();
		glPopMatrix();
	}
}
