#include "Global.h"
#include "AnnotationPoint.h"

#include "../api/AP.h"

// #ifdef _WIN32
// #include <windows.h>
// #endif
// #include <GL/gl.h>

#include "MainApplication.h"

#include "../renderers/IAnnotationPointRenderer.h"

CAnnotationPoint::CAnnotationPoint(std::shared_ptr<CBaseObject> m) :CAnnotation(m), CPoint3d()
{
	setLabel("point");
	m_direction.Set(0, 0, 0);
	m_faceId = -1;
	m_showNorm = true;
	renderer_ = std::make_shared<IAnnotationPointRenderer>();
}

CAnnotationPoint::CAnnotationPoint(int objId) :CAnnotation(objId), CPoint3d()
{
	setLabel("point");
	m_direction.Set(0, 0, 0);
	m_faceId = -1;
	m_showNorm = true;
	renderer_ = std::make_shared<IAnnotationPointRenderer>();
}

CAnnotationPoint::CAnnotationPoint(CAnnotationPoint& t) :CAnnotation(t), CPoint3d(t)
{
	m_faceId = t.m_faceId;
	m_faceA = t.m_faceA;
	m_faceB = t.m_faceB;
	m_faceC = t.m_faceC;
	m_direction = t.m_direction;
	m_showNorm = true;
	renderer_ = std::make_shared<IAnnotationPointRenderer>();
}

CAnnotationPoint::CAnnotationPoint(const double& tx, const double& ty, const double& tz) :CAnnotation(-1), CPoint3d(tx, ty, tz)
{
	//m_point.Set(tx, ty, tz);
	setLabel("point");
	m_direction.Set(0, 0, 0);
	m_faceId = -1;
	m_showNorm = true;
	renderer_ = std::make_shared<IAnnotationPointRenderer>();
}

CAnnotationPoint::CAnnotationPoint(const CPoint3d& t) :CAnnotation(-1), CPoint3d(t)
{
	//m_point.Set( t );
	setLabel("point");
	m_direction.Set(0, 0, 0);
	m_faceId = -1;
	m_showNorm = true;
	renderer_ = std::make_shared<IAnnotationPointRenderer>();
}

CAnnotationPoint::CAnnotationPoint(const CPoint3d& t, const CPoint3d& t0) :CAnnotation(-1), CPoint3d(t)
{
	//m_point.Set(t);

	setLabel("point");
	m_direction = CVector3d(t0, t);
	m_direction.normalize();

	m_faceId = -1;
	m_showNorm = true;
	renderer_ = std::make_shared<IAnnotationPointRenderer>();
}

CAnnotationPoint::CAnnotationPoint(const CPoint3d& t, const CRGBA kol) :CAnnotation(-1), CPoint3d(t)
{
	//m_point.Set( t );
	setLabel("point");
	m_direction.Set(0, 0, 0);
	m_faceId = -1;
	m_showNorm = true;
	renderer_ = std::make_shared<IAnnotationPointRenderer>();
}


//void CAnnotationPoint::applyTransform(CTransform prevT, CTransform newT)
//{
//	CPoint3d p0( *this );
//	CPoint3d p1( p0 + m_direction );
//	
//	p0 = newT.w2l(prevT.l2w(p0));
//	this->Set(p0);
//
//	if (m_direction.length())
//	{
//		p1 = newT.w2l(prevT.l2w(p1));
//
//		m_direction = CVector3d(p0, p1).getNormalized();
//	}
//}


void CAnnotationPoint::applyTransformation(CTransform& from, CTransform& to)
{
	CPoint3d p0 = to.w2l(from.l2w(*this));
	CPoint3d p1 = to.w2l(from.l2w(*this + m_direction));

	*this = p0;
	m_direction = p1 - p0;
	m_direction.normalize();
}

std::wstring CAnnotationPoint::getInfoRow()
{
	std::wstring info = L"Coordinates: [ X=" + std::to_wstring((long double)x) + L", Y=" + std::to_wstring((long double)y) + L", Z=" + std::to_wstring((long double)z) + L" ]";
	info.append( L", vector: [ vX=" + std::to_wstring((long double)m_direction.X()) + L", vY=" + std::to_wstring((long double)m_direction.Y()) + L", vZ=" + std::to_wstring((long double)m_direction.Z()) + L" ]" );

	info.append( L", selected face: Id(" + std::to_wstring((long long)m_faceId) + L"): " );
	info.append( L"[ " + std::to_wstring((long double)m_faceA.X()) + L", " + std::to_wstring((long double)m_faceA.Y()) + L", " + std::to_wstring((long double)m_faceA.Z()) + L" ] - " );
	info.append( L"[ " + std::to_wstring((long double)m_faceB.X()) + L", " + std::to_wstring((long double)m_faceB.Y()) + L", " + std::to_wstring((long double)m_faceB.Z()) + L" ] - " );
	info.append( L"[ " + std::to_wstring((long double)m_faceC.X()) + L", " + std::to_wstring((long double)m_faceC.Y()) + L", " + std::to_wstring((long double)m_faceC.Z()) + L" ]" );

	return info;
}

