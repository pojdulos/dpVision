#include "Global.h"
#include "AnnotationCurvatureMap.h"

//#include "AP.h"

#ifdef _WIN32
#include <windows.h>
#endif
//#include <GL/gl.h>

#include "../renderers/IAnnotationCurvatureMapRenderer.h"

CAnnotationCurvatureMap::CAnnotationCurvatureMap(std::shared_ptr<CBaseObject> parent) :CAnnotation(parent), CCurvatureMap()
{
	setLabel("curvature map");
	renderer_ = std::make_shared<IAnnotationCurvatureMapRenderer>();
}

std::set<INDEX_TYPE> CAnnotationCurvatureMap::dajSiodlo()
{
	std::set<INDEX_TYPE> siodlow;
	for (auto el : *this)
	{
		if ((el.second.KG < 0) && (el.second.Kmean < 0)) // siodlo wnetrze
		{
			siodlow.insert(el.first);
		}
	}

	return siodlow;
}

std::wstring CAnnotationCurvatureMap::getInfoRow()
{
	std::wstring info = L"Size: " + std::to_wstring((long long)CCurvatureMap::size()) + L" elements.";

	return info;
}

