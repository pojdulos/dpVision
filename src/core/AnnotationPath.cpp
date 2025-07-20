#include "Global.h"
#include "AnnotationPath.h"

#include "../api/AP.h"

// #ifdef _WIN32
// #include <windows.h>
// #endif
// #include <GL/gl.h>


#include "../renderers/IAnnotationPathRenderer.h"

CAnnotationPath::CAnnotationPath(std::shared_ptr<CBaseObject> m) :CAnnotation(m)
{
	setLabel("path");

	renderer_ = std::make_shared<IAnnotationPathRenderer>();
}

CAnnotationPath::iterator CAnnotationPath::cyclicNext(CAnnotationPath::iterator current)
{
	CAnnotationPath::iterator next = current;

	next++;
	if (next == end()) next = begin();

	return next;
}

CAnnotationPath::iterator CAnnotationPath::cyclicPrev(CAnnotationPath::iterator current)
{
	CAnnotationPath::iterator prev = current;

	if (prev == begin()) prev = end();
	prev--;

	return CAnnotationPath::iterator();
}

std::wstring CAnnotationPath::getInfoRow()
{
	std::wstring info = L"Size: " + std::to_wstring((long long)size()) + L" points.";

	return info;
}

