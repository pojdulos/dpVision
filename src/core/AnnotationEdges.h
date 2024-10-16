#pragma once
#include "Annotation.h"
#include "Punkt3D.h"
#include "Wektor3D.h"
#include "RGBA.h"
#include "Model3D.h"
#include "Edge.h"

#include <list>

struct  CAnnotationEdge 
{
	CPoint3f first;
	CPoint3f second;
	std::set<size_t> faces;
};


class DPVISION_EXPORT CAnnotationEdges : public CAnnotation
{
public:
	std::list<CAnnotationEdge> m_list;

	CAnnotationEdges( CModel3D *m = nullptr) :CAnnotation( m )
	{
		setLabel("edge_set");
	};

	CAnnotationEdges( CAnnotationEdges &p ) :CAnnotation( p )
	{
		setLabel("edge_set");
		m_list = p.m_list;
	};

	CAnnotationEdges( std::list<CAnnotationEdge> &list, CModel3D *m = nullptr) :CAnnotation(m)
	{
		setLabel("edge_set");
		m_list = list;
	};

	~CAnnotationEdges() { m_list.clear(); };
	
	void addEdge(CPoint3f f, CPoint3f s);
	void addEdge(CPoint3f f, CPoint3f s, size_t fId);
//	void addEdge(CEdge ed);

	virtual int type() { return CAnnotation::EDGES; }

	virtual std::wstring getTypeWSTR() { return L"edge_set"; };
	virtual std::wstring getInfoRow();
	virtual void renderSelf() override;
};

//class  DPVISION_EXPORT CPathsMap : public std::map<int, CAnnotationPath*> {};
