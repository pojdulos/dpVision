#include "Global.h"
#include "AnnotationBox.h"

#include "../api/AP.h"

// #ifdef _WIN32
// #include <windows.h>
// #endif
// #include <GL/gl.h>

#include "../renderers/IAnnotationBoxRenderer.h"

CAnnotationBox::CAnnotationBox(std::shared_ptr<CBaseObject> m) :CAnnotation(m)
{
	setLabel("box");

	renderer_ = std::make_shared<IAnnotationBoxRenderer>();
}

CAnnotationBox::CAnnotationBox(CAnnotationBox& p) : CAnnotation(p)
{
	setLabel("box");
	//m_min = p.m_min;
	//m_max = p.m_max;

	renderer_ = std::make_shared<IAnnotationBoxRenderer>();
}

std::wstring CAnnotationBox::getInfoRow()
{
	std::wstring info = L"";// MinSize: " + std::to_wstring((long long)m_pairs.size());

	return info;
}

