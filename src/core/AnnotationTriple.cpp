#include "Global.h"
#include "AnnotationTriple.h"

#include "AP.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#include "../renderers/IAnnotationTripleRenderer.h"

CAnnotationTriple::CAnnotationTriple(std::shared_ptr<CBaseObject> m) :CAnnotation(m)
{
	m_origin = CPoint3d(0.0, 0.0, 0.0);
	m_rot = CVector3d(0.0, 0.0, 0.0);

	setLabel("triple");
	renderer_ = std::make_shared<IAnnotationTripleRenderer>();
}

CAnnotationTriple::CAnnotationTriple(CAnnotationTriple& t) :CAnnotation(t)
{
	m_origin = t.m_origin;
	m_rot = t.m_rot;

	setLabel("triple");
	m_front = t.m_front;
	m_left = t.m_left;
	m_right = t.m_right;
	renderer_ = std::make_shared<IAnnotationTripleRenderer>();
}

CPoint3d CAnnotationTriple::realPos(CPoint3d p)
{
	CTransform t;
	t.rotateAroundAxisDeg(CVector3d::XAxis(), m_rot.X());
	t.rotateAroundAxisDeg(CVector3d::YAxis(), m_rot.Y());
	t.rotateAroundAxisDeg(CVector3d::ZAxis(), m_rot.Z());

	t.translate(CVector3d(CPoint3d(0, 0, 0), m_origin));

	return t.l2w(p);
}

void CAnnotationTriple::applyTransform(CTransform prevT, CTransform newT)
{
	m_front = newT.w2l(prevT.l2w(m_front));
	m_left = newT.w2l(prevT.l2w(m_left));
	m_right = newT.w2l(prevT.l2w(m_right));
}

std::wstring CAnnotationTriple::getInfoRow()
{
	std::wstring info = L"F: [" + std::to_wstring((double) m_front.X()) + L"," + std::to_wstring((double)m_front.Y()) + L"," + std::to_wstring((double)m_front.Z()) + L"]\n";
	info.append(L"L: [" + std::to_wstring((double)m_left.X()) + L"," + std::to_wstring((double)m_left.Y()) + L"," + std::to_wstring((double)m_left.Z()) + L"]\n");
	info.append(L"R: [" + std::to_wstring((double)m_right.X()) + L"," + std::to_wstring((double)m_right.Y()) + L"," + std::to_wstring((double)m_right.Z()) + L"]\n");

	//info.append( L", selected face: Id(" + std::to_wstring((long long)m_faceId) + L"): " );
	//info.append( L"[ " + std::to_wstring((long double)m_faceA.X()) + L", " + std::to_wstring((long double)m_faceA.Y()) + L", " + std::to_wstring((long double)m_faceA.Z()) + L" ] - " );
	//info.append( L"[ " + std::to_wstring((long double)m_faceB.X()) + L", " + std::to_wstring((long double)m_faceB.Y()) + L", " + std::to_wstring((long double)m_faceB.Z()) + L" ] - " );
	//info.append( L"[ " + std::to_wstring((long double)m_faceC.X()) + L", " + std::to_wstring((long double)m_faceC.Y()) + L", " + std::to_wstring((long double)m_faceC.Z()) + L" ]" );

	return info;
}

void CAnnotationTriple::drawAxes()
{
	glLineWidth(3);
	glBegin(GL_LINES);
	glColor3f(0.2f, 0.2f, 0.8f);
	glVertex3dv(realPos(CPoint3d(0, 0, 0)).toVector()); glVertex3dv(realPos(CPoint3d(10,  0, 0)).toVector());
	glColor3f(0.2f, 0.8f, 0.2f);
	glVertex3dv(realPos(CPoint3d(0, 0, 0)).toVector()); glVertex3dv(realPos(CPoint3d( 0, 10, 0)).toVector());
	glColor3f(0.8f, 0.2f, 0.2f);
	glVertex3dv(realPos(CPoint3d(0, 0, 0)).toVector()); glVertex3dv(realPos(CPoint3d( 0, 0, 10)).toVector());
	glEnd();
}

void CAnnotationTriple::renderSelf()
{
	glPushMatrix();
	glPushAttrib(GL_ALL_ATTRIB_BITS);

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	drawAxes();

	if (m_checked)
	{
		glColor4ubv(m_selcolor.V());
	}
	else
	{
		glColor4ubv(m_color.V());
	}

	//glLineWidth(5);

	glBegin(GL_TRIANGLES);
		glVertex3dv(realPos(m_front).toVector());
		glVertex3dv(realPos(m_left).toVector());
		glVertex3dv(realPos(m_right).toVector());
	glEnd();



	glPointSize(5);

	if (m_checked)
	{
		glColor4ubv(m_selcolor.V());
	}
	else
	{
		glColor4ubv(m_color.V());
	}

	glEnable(GL_POINT_SMOOTH);
	glBegin(GL_POINTS);
	glVertex3dv(realPos(m_front).toVector());
	glVertex3dv(realPos(m_left).toVector());
	glVertex3dv(realPos(m_right).toVector());
	glEnd();


	//if ( 0.0 != m_direction.length() )
	//{
	//	glLineWidth(1);
	//	glColor3f(1.0, 1.0, 0.0);
	//	glBegin(GL_LINES);
	//	glVertex3f(-5*this->m_direction.X(), -5*this->m_direction.Y(), -5*this->m_direction.Z());
	//	glVertex3f(0, 0, 0);
	//	glEnd();
	//}

	glPopAttrib();
	glPopMatrix();
}
