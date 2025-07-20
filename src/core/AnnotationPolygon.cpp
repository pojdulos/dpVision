#include "Global.h"
#include "AnnotationPolygon.h"

#include "AP.h"

// #ifdef _WIN32
// #include <windows.h>
// #endif
// #include <GL/gl.h>

#include "../renderers/IAnnotationPolygonRenderer.h"

CAnnotationPolygon::CAnnotationPolygon(std::shared_ptr<CBaseObject> m) :CAnnotation(m)
{
	setLabel("polygon");
	renderer_ = std::make_shared<IAnnotationPolygonRenderer>();
}

CAnnotationPolygon::CAnnotationPolygon(CAnnotationPolygon& p) : CAnnotation(p)
{
	setLabel("polygon");
	m_data = p.m_data;
	renderer_ = std::make_shared<IAnnotationPolygonRenderer>();
}

std::wstring CAnnotationPolygon::getInfoRow()
{
	std::wstring info = L"Size: " + std::to_wstring((long long)m_data.size());

	return info;
}

