#include "Global.h"
#include "AnnotationSphere.h"

#include "../api/AP.h"

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


