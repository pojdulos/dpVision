#include "Global.h"
#include "AnnotationPolygon2D.h"

#include "../api/AP.h"
#include "Model3D.h"
#include "Mesh.h"

//#include <Windows.h>
#include <GL/gl.h>

#include "../renderers/IAnnotationPolygon2DRenderer.h"

CAnnotationPolygon2D::CAnnotationPolygon2D(std::shared_ptr<CBaseObject> m) :CAnnotation(m)
{
	setLabel("polygon2d");
	renderer_ = std::make_shared<IAnnotationPolygon2DRenderer>();
}

CAnnotationPolygon2D::CAnnotationPolygon2D(CAnnotationPolygon2D& p) : CAnnotation(p)
{
	setLabel("polygon2d");
	m_pairs = p.m_pairs;
	renderer_ = std::make_shared<IAnnotationPolygon2DRenderer>();
}

std::wstring CAnnotationPolygon2D::getInfoRow()
{
	std::wstring info = L"Size: " + std::to_wstring((long long)m_pairs.size());

	return info;
}

