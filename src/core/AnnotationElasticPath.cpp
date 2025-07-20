#include "Global.h"
#include "AnnotationElasticPath.h"

#include "../api/AP.h"

// #ifdef _WIN32
// #include <windows.h>
// #endif
// #include <GL/gl.h>

#include "../renderers/IAnnotationElasticPathRenderer.h"

CAnnotationElasticPath::CAnnotationElasticPath() {
	setLabel("path");
	setParent(nullptr);
	setColor(CRGBA(1.0f, 0.0f, 0.0f, 1.0f));
	m_checked = false; 
	renderer_ = std::make_shared<IAnnotationElasticPathRenderer>();
}

CAnnotationElasticPath::CAnnotationElasticPath(std::shared_ptr<CModel3D> m) {
	setLabel("path");
	m_parent = m;
	m_color = CRGBA(1.0f, 0.0f, 0.0f, 1.0f);
	m_checked = false; 
	renderer_ = std::make_shared<IAnnotationElasticPathRenderer>();
}

std::wstring CAnnotationElasticPath::getInfoRow()
{
	std::wstring info = L"Size: " + std::to_wstring((long long) m_data.size()) + L" points.";

	return info;
}

