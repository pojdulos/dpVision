#include "Global.h"
#include "AnnotationPodkowa.h"

#include "../api/AP.h"

// #ifdef _WIN32
// #include <windows.h>
// #endif
// #include <GL/gl.h>
// #include <GL/glu.h> 

#include <math.h>

#include "../renderers/IAnnotationPodkowaRenderer.h"

CAnnotationPodkowa::CAnnotationPodkowa(std::shared_ptr<CBaseObject> m) :CAnnotationCylinder(m)
{
	setLabel("podkowa");
	m_arm_length = 50.0;
	renderer_ = std::make_shared<IAnnotationPodkowaRenderer>();
}

CAnnotationPodkowa::CAnnotationPodkowa(CAnnotationPodkowa& t) :CAnnotationCylinder(t)
{
	setLabel("podkowa");
	m_arm_length = t.m_arm_length;
	renderer_ = std::make_shared<IAnnotationPodkowaRenderer>();
}

std::wstring CAnnotationPodkowa::getInfoRow()
{
	std::wstring info = L"Podkowa. R="+ std::to_wstring(m_radius) + L", a=" + std::to_wstring(m_start_angle) + L", L=" + std::to_wstring(m_arm_length);
	
	return info;
}


// std::list<CPoint3f[3]> CAnnotationPodkowa::getFaces()
// {

// 	return std::list<CPoint3f[3]>();
// }

