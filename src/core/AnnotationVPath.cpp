#include "Global.h"
#include "AnnotationVPath.h"

#include "AP.h"

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>


#include "../renderers/IAnnotationVPathRenderer.h"

CAnnotationVPath::CAnnotationVPath(std::shared_ptr<CBaseObject> m) :CAnnotation(m), ListOfVIndices()
{
	setLabel("v_path");
	renderer_ = std::make_shared<IAnnotationVPathRenderer>();
}

CAnnotationVPath::CAnnotationVPath(const CAnnotationVPath& p) : CAnnotation(p), ListOfVIndices(p)
{
	setLabel("v_path");
	//m_list = p.m_list;
	m_length = p.m_length;
	renderer_ = std::make_shared<IAnnotationVPathRenderer>();
}

CAnnotationVPath::CAnnotationVPath(ListOfVIndices& list, double d, std::shared_ptr<CBaseObject> m) :CAnnotation(m), ListOfVIndices(list)
{
	setLabel("v_path");
	//m_list = list;
	m_length = d;
	renderer_ = std::make_shared<IAnnotationVPathRenderer>();
}

CAnnotationVPath::CAnnotationVPath(ListOfVIndices& list, std::shared_ptr<CBaseObject> m) :CAnnotation(m), ListOfVIndices(list)
{
	setLabel("v_path");
	if (auto o = std::dynamic_pointer_cast<CObject>(m))
	{
		if (auto mesh = std::dynamic_pointer_cast<CMesh>(o->getChild()))
		{
			m_length = 0.0;
			ListOfVIndices::iterator it = begin();
			size_t v1 = *(it++);

			while (it != end())
			{
				INDEX_TYPE v2 = *(it++);
				m_length += CVector3<double>(mesh->vertices()[v1], mesh->vertices()[v2]).length();
				v1 = v2;

			}
		}
	}
	renderer_ = std::make_shared<IAnnotationVPathRenderer>();
}


std::wstring CAnnotationVPath::getInfoRow()
{
	std::wstring info = L"Length: " + std::to_wstring(m_length) + L" Size: " + std::to_wstring((long long) size()) + L" points.";

	return info;
}



ListOfVIndices::iterator CAnnotationVPath::cyclicNext(ListOfVIndices::iterator current)
{
	ListOfVIndices::iterator next = current;

	next++;
	if (next == end()) next = begin();

	return next;
}

ListOfVIndices::iterator CAnnotationVPath::cyclicPrev(ListOfVIndices::iterator current)
{
	ListOfVIndices::iterator prev = current;

	if (prev == begin()) prev = end();
	prev--;

	return prev;
}
