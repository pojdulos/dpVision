#include "Global.h"
#include "AnnotationSphere.h"

#include "AP.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#include "../renderers/IAnnotationSphereRenderer.h"

CAnnotationSphere::CAnnotationSphere(std::shared_ptr<CBaseObject> m) :Kula(0.0, 0.0, 0.0, 1.0), CAnnotation(m)
{
	m_lats = m_longs = 128;
	setLabel("sphere");
	renderer_ = std::make_shared<IAnnotationSphereRenderer>();
}

CAnnotationSphere::CAnnotationSphere(CAnnotationSphere& t) :Kula(t.center(), t.radius()), CAnnotation(t)
{
	m_lats = t.m_lats;
	m_longs = t.m_longs;
	renderer_ = std::make_shared<IAnnotationSphereRenderer>();
}

CAnnotationSphere::CAnnotationSphere(Kula& k) :Kula(k.center(), k.radius()), CAnnotation(nullptr)
{
	m_lats = m_longs = 128;
	setLabel("sphere");
	renderer_ = std::make_shared<IAnnotationSphereRenderer>();
}

void CAnnotationSphere::applyTransform(CTransform prevT, CTransform newT)
{
	//m_front = newT.w2l(prevT.l2w(m_front));
	//m_left = newT.w2l(prevT.l2w(m_left));
	//m_right = newT.w2l(prevT.l2w(m_right));
}

std::wstring CAnnotationSphere::getInfoRow()
{
	std::wstring info = L"C: [" + std::to_wstring(m_c.x) + L"," + std::to_wstring(m_c.y) + L"," + std::to_wstring(m_c.z) + L"]\n";
	info.append(L"R: " + std::to_wstring(m_r) + L"\n");

	return info;
}


void CAnnotationSphere::drawSphere(int lats, int longs) {
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

void CAnnotationSphere::renderSelf()
{
	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	//drawAxes();

	if (m_selected)
	{
		glColor4ubv(m_selcolor.V());
	}
	else
	{
		glColor4ubv(m_color.V());
	}

	drawSphere(m_lats, m_longs);

	glPopAttrib();
	glPopMatrix();
}
