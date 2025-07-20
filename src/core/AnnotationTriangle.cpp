#include "Global.h"
#include "AnnotationTriangle.h"

#include "AP.h"

// #ifdef _WIN32
// #include <windows.h>
// #endif
// #include <GL/gl.h>

#include "../renderers/IAnnotationTriangleRenderer.h"

CAnnotationTriangle::CAnnotationTriangle(std::shared_ptr<CBaseObject> m) :CAnnotation(m)
{
	setLabel("triple");
	renderer_ = std::make_shared<IAnnotationTriangleRenderer>();
}

CAnnotationTriangle::CAnnotationTriangle(CAnnotationTriangle& t) : CAnnotation(t)
{
	m_pC = t.m_pC;
	m_pA = t.m_pA;
	m_pB = t.m_pB;
	renderer_ = std::make_shared<IAnnotationTriangleRenderer>();
}

CAnnotationTriangle::CAnnotationTriangle(CTriangle& t) :CAnnotation(nullptr)
{
	setLabel("triple");
	m_pC = t[2];
	m_pA = t[0];
	m_pB = t[1];
	renderer_ = std::make_shared<IAnnotationTriangleRenderer>();
}

CAnnotationTriangle::CAnnotationTriangle(CPoint3d A, CPoint3d B, CPoint3d C) :CAnnotation(nullptr)
{
	setLabel("triple");
	m_pA = A;
	m_pB = B;
	m_pC = C;
	renderer_ = std::make_shared<IAnnotationTriangleRenderer>();
}

CAnnotationTriangle::CAnnotationTriangle(CPoint3d v[3]) :CAnnotation(nullptr)
{
	setLabel("triple");
	m_pA = v[0];
	m_pB = v[1];
	m_pC = v[2];
	renderer_ = std::make_shared<IAnnotationTriangleRenderer>();
}

void CAnnotationTriangle::applyTransform(CTransform prevT, CTransform newT)
{
	m_pC = newT.w2l(prevT.l2w(m_pC));
	m_pA = newT.w2l(prevT.l2w(m_pA));
	m_pB = newT.w2l(prevT.l2w(m_pB));
}

std::wstring CAnnotationTriangle::getInfoRow()
{
	std::wstring info = L"A: [" + std::to_wstring((double) m_pA.X()) + L"," + std::to_wstring((double)m_pA.Y()) + L"," + std::to_wstring((double)m_pA.Z()) + L"]\n";
	info.append(L"B: [" + std::to_wstring((double)m_pB.X()) + L"," + std::to_wstring((double)m_pB.Y()) + L"," + std::to_wstring((double)m_pB.Z()) + L"]\n");
	info.append(L"C: [" + std::to_wstring((double)m_pC.X()) + L"," + std::to_wstring((double)m_pC.Y()) + L"," + std::to_wstring((double)m_pC.Z()) + L"]\n");

	return info;
}

