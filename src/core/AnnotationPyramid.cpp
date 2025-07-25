#include "Global.h"
#include "AnnotationPyramid.h"

#include "../api/AP.h"

#include "../renderers/IAnnotationPyramidRenderer.h"

CAnnotationPyramid::CAnnotationPyramid(std::shared_ptr<CBaseObject> m) :CAnnotation(m)
{
	setLabel("Pyramid");
	renderer_ = std::make_shared<IAnnotationPyramidRenderer>();
}

CAnnotationPyramid::CAnnotationPyramid(CAnnotationPyramid& p) : CAnnotation(p)
{
	setLabel("Pyramid");
	m_top[0] = p.m_top[0];
	m_top[1] = p.m_top[1];
	m_top[2] = p.m_top[2];
	m_top[3] = p.m_top[3];
	m_bottom[0] = p.m_bottom[0];
	m_bottom[1] = p.m_bottom[1];
	m_bottom[2] = p.m_bottom[2];
	m_bottom[3] = p.m_bottom[3];
	renderer_ = std::make_shared<IAnnotationPyramidRenderer>();
}

std::wstring CAnnotationPyramid::getInfoRow()
{
	std::wstring info = L"Pyramid";

	return info;
}
