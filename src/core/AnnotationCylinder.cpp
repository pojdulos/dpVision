#include "Global.h"
#include "AnnotationCylinder.h"

#include "../api/AP.h"

#include "../renderers/IAnnotationCylinderRenderer.h"

CAnnotationCylinder::CAnnotationCylinder(std::shared_ptr<CBaseObject> m) :CAnnotation(m)
{
	setLabel("cylinder");
	m_point = CPoint3f(0, 0, 0);
	m_radius = 0;
	m_halfLength = 0;
	m_start_angle = 0.0;
	m_end_angle = 360.0;
	renderer_ = std::make_shared<IAnnotationCylinderRenderer>();
}

CAnnotationCylinder::CAnnotationCylinder(CAnnotationCylinder& t) :CAnnotation(t)
{
	setLabel("cylinder");
	m_point = t.m_point;
	m_radius = t.m_radius;
	m_halfLength = t.m_halfLength;
	m_start_angle = t.m_start_angle;
	m_end_angle = t.m_end_angle;
	renderer_ = std::make_shared<IAnnotationCylinderRenderer>();
}

std::wstring CAnnotationCylinder::getInfoRow()
{
	std::wstring info = L"Cylinder";
	
	return info;
}

