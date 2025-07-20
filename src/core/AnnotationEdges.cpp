#include "Global.h"
#include "AnnotationEdges.h"

#include "AP.h"

// #ifdef _WIN32
// #include <windows.h>
// #endif
// #include <GL/gl.h>


#include "../renderers/IAnnotationEdgesRenderer.h"

CAnnotationEdges::CAnnotationEdges(std::shared_ptr<CBaseObject> m) :CAnnotation(m)
{
	setLabel("edge_set");
	renderer_ = std::make_shared<IAnnotationEdgesRenderer>();
}

CAnnotationEdges::CAnnotationEdges(CAnnotationEdges& p) : CAnnotation(p)
{
	setLabel("edge_set");
	m_list = p.m_list;
	renderer_ = std::make_shared<IAnnotationEdgesRenderer>();
}

CAnnotationEdges::CAnnotationEdges(std::list<CAnnotationEdge>& list, std::shared_ptr<CBaseObject> m) :CAnnotation(m)
{
	setLabel("edge_set");
	m_list = list;
	renderer_ = std::make_shared<IAnnotationEdgesRenderer>();
}



void CAnnotationEdges::addEdge(CPoint3f f, CPoint3f s, size_t fId)
{
	CAnnotationEdge e;
	e.first = f;
	e.second = s;
	e.faces.insert(fId);

	m_list.push_back(e);
}

void CAnnotationEdges::addEdge(CPoint3f f, CPoint3f s)
{
	CAnnotationEdge e;
	e.first = f;
	e.second = s;

	m_list.push_back(e);
}

//void CAnnotationEdges::addEdge(CEdge ed)
//{
//	CAnnotationEdge e;
//	e.first = ed.first;
//	e.second = ed.seconds;
//	e.faces.insert(ed.face());
//
//	m_list.push_back(e);
//}

std::wstring CAnnotationEdges::getInfoRow()
{
	std::wstring info = L"Size: " + std::to_wstring((long long) m_list.size()) + L" edges.";

	return info;
}

