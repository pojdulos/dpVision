#include "Global.h"
#include "AnnotationPoints.h"

#include "../api/AP.h"

#include "../renderers/IAnnotationPointsRenderer.h"

CAnnotationPoints::CAnnotationPoints(std::shared_ptr<CBaseObject> m) :CAnnotation(m)
{
	setLabel("points");
	m_list.clear();
	renderer_ = std::make_shared<IAnnotationPointsRenderer>();
}

void CAnnotationPoints::applyTransformation(CTransform &prevT, CTransform &newT)
{
	for (SetOfPoints::iterator ip = this->m_list.begin(); ip != this->m_list.end(); ip++)
	{
		*ip = newT.w2l(prevT.l2w(*ip));
	}
}

std::wstring CAnnotationPoints::getInfoRow()
{
	std::wstring info = L"Size: " + std::to_wstring((long long) m_list.size()) + L" points.";

	return info;
}

