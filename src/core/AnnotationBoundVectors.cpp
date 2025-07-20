#include "Global.h"
#include "AnnotationBoundVectors.h"

#include "../api/AP.h"

// #ifdef _WIN32
// #include <windows.h>
// #endif
// #include <GL/gl.h>

#include "../renderers/IAnnotationBoundVectorsRenderer.h"

CAnnotationBoundVectors::CAnnotationBoundVectors(std::shared_ptr<CBaseObject> m) :CAnnotation(m)
{
	setLabel("bound_vectors");
	m_data.clear();

	renderer_ = std::make_shared<IAnnotationBoundVectorsRenderer>();
}

void CAnnotationBoundVectors::applyTransformation(CTransform &prevT, CTransform &newT)
{
	for ( std::list<_BoundVector>::iterator ip = this->m_data.begin(); ip != this->m_data.end(); ip++ )
	{
		ip->first = newT.w2l(prevT.l2w(ip->first));
		ip->second = newT.w2l(prevT.l2w(ip->second));
	}
}

std::wstring CAnnotationBoundVectors::getInfoRow()
{
	std::wstring info = L"Size: " + std::to_wstring((long long) m_data.size()) + L" vectors.";

	return info;
}
