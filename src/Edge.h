#pragma once
#include "Global.h"

#include <set>
#include <list>
#include <vector>

class __declspec(dllexport) CEdge : public std::pair<INDEX_TYPE, INDEX_TYPE>
{
public:
	INDEX_TYPE face;
	std::set<INDEX_TYPE> faces;

	CEdge()
	{
		first = 0;
		second = 0;
	}

	CEdge(INDEX_TYPE f, INDEX_TYPE s)
	{
		//if (first < second)
		//{
			first = f;
			second = s;
		//}
		//else
		//{
		//	first = s;
		//	second = f;
		//}

		face = 0;
	};

	CEdge(INDEX_TYPE f, INDEX_TYPE s, INDEX_TYPE t)
	{
		//if (first < second)
		//{
			first = f;
			second = s;
		//}
		//else
		//{
		//	first = s;
		//	second = f;
		//}

		face = t;
	};

	CEdge( const CEdge &e )
	{
		//if (e.first < e.second)
		//{
			first = e.first;
			second = e.second;
		//}
		//else
		//{
		//	first = e.second; 
		//	second = e.first;
		//}

		face = e.face;
		faces = e.faces;
	};

	bool isValid() { return (first != second) && !faces.empty() && (faces.size() < 2); }
	//bool isBoundary() { return faces.size() == 1; }
	bool hasUnexpectedFaces() { return faces.size() > 1; }

	void addFace(INDEX_TYPE idx);
	bool hasFace(INDEX_TYPE idx) { return faces.find(idx) != faces.end(); }

};


class __declspec(dllexport)  CSetOfEdges : public std::set< CEdge >
{
public:
	CSetOfEdges::iterator findFirstVertex(INDEX_TYPE index);
	CSetOfEdges::iterator findSecondVertex(INDEX_TYPE index);

	CSetOfEdges::iterator find(INDEX_TYPE f, INDEX_TYPE s);
	CSetOfEdges::iterator find(CEdge e);
};


class __declspec(dllexport)  CListOfEdges : public std::list< CEdge > {
public:
	CListOfEdges::iterator findFirstVertex(INDEX_TYPE index);
	CListOfEdges::iterator findSecondVertex(INDEX_TYPE index);
};

