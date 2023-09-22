#include "Global.h"
#include "AnnotationCurvatureMap.h"

//#include "AP.h"

//#include <Windows.h>
//#include <GL/GL.h>

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

