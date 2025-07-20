#include "Global.h"
#include "AnnotationPlane.h"

#include "../api/AP.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#include "../renderers/IAnnotationPlaneRenderer.h"

CAnnotationPlane::CAnnotationPlane(std::shared_ptr<CBaseObject> m) :CAnnotation(m), CPlane()
{
	setLabel("plane");
	m_color.SetFloat(0.0f, 1.0f, 1.0f, 0.5f);
	m_selcolor.SetFloat(1.0f, 0.0f, 0.0f, 0.5f);

	m_size = 10;

	renderer_ = std::make_shared<IAnnotationPlaneRenderer>();
}

CAnnotationPlane::CAnnotationPlane(int objId) :CAnnotation(objId), CPlane()
{
	setLabel("plane");
	m_color.SetFloat(0.0f, 1.0f, 1.0f, 0.5f);
	m_selcolor.SetFloat(1.0f, 0.0f, 0.0f, 0.5f);
	m_size = 10;

	renderer_ = std::make_shared<IAnnotationPlaneRenderer>();
}

CAnnotationPlane::CAnnotationPlane(CAnnotationPlane& t) :CAnnotation(t), CPlane(t)
{
	//setLabel("plane");
	//m_color.SetFloat(0.0f, 1.0f, 1.0f, 0.5f);
	//m_selcolor.SetFloat(1.0f, 0.0f, 0.0f, 0.5f);
	m_size = t.m_size;

	renderer_ = std::make_shared<IAnnotationPlaneRenderer>();
}

CAnnotationPlane::CAnnotationPlane(CPlane p) :CAnnotation(-1), CPlane(p)
{
	setLabel("plane");
	m_normal.normalize();
	m_color.SetFloat(0.0f, 1.0f, 1.0f, 0.5f);
	m_selcolor.SetFloat(1.0f, 0.0f, 0.0f, 0.5f);
	m_size = 10;

	renderer_ = std::make_shared<IAnnotationPlaneRenderer>();
}

CAnnotationPlane::CAnnotationPlane(CPoint3d p, CVector3d n) :CAnnotation(-1), CPlane(p, n)
{
	setLabel("plane");
	m_normal.normalize();
	m_color.SetFloat(0.0f, 1.0f, 1.0f, 0.5f);
	m_selcolor.SetFloat(1.0f, 0.0f, 0.0f, 0.5f);
	m_size = 10;

	renderer_ = std::make_shared<IAnnotationPlaneRenderer>();
}

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

