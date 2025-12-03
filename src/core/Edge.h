#pragma once
#include "dll_global.h"

#include "Global.h"

#include <set>
#include <list>
#include <vector>

class DPVISION_EXPORT CEdge : public std::pair<INDEX_TYPE, INDEX_TYPE>
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

	CEdge(std::pair<const INDEX_TYPE&, const INDEX_TYPE&> p)
	{
		//if (first < second)
		//{
		first = p.first;
		second = p.second;
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

	// **KLUCZOWE: Operator < z normalizacj¹**
	bool operator<(const CEdge& other) const {
		INDEX_TYPE a1 = std::min(first, second);
		INDEX_TYPE b1 = std::max(first, second);
		INDEX_TYPE a2 = std::min(other.first, other.second);
		INDEX_TYPE b2 = std::max(other.first, other.second);

		if (a1 != a2) return a1 < a2;
		return b1 < b2;
	}

	// **OPCJONALNIE: Operator == z normalizacj¹**
	bool operator==(const CEdge& other) const {
		INDEX_TYPE a1 = std::min(first, second);
		INDEX_TYPE b1 = std::max(first, second);
		INDEX_TYPE a2 = std::min(other.first, other.second);
		INDEX_TYPE b2 = std::max(other.first, other.second);

		return (a1 == a2) && (b1 == b2);
	}

	bool isValid() { return (first != second) && !faces.empty() && (faces.size() < 2); }
	//bool isBoundary() { return faces.size() == 1; }
	bool hasUnexpectedFaces() { return faces.size() > 1; }

	void addFace(INDEX_TYPE idx);
	bool hasFace(INDEX_TYPE idx) { return faces.find(idx) != faces.end(); }

};


class DPVISION_EXPORT  CSetOfEdges : public std::set< CEdge >
{
public:
	CSetOfEdges::iterator findFirstVertex(INDEX_TYPE index);
	CSetOfEdges::iterator findSecondVertex(INDEX_TYPE index);

	CSetOfEdges::iterator find(INDEX_TYPE f, INDEX_TYPE s);
	CSetOfEdges::iterator find(CEdge e);
};


class DPVISION_EXPORT  CListOfEdges : public std::list< CEdge > {
public:
	CListOfEdges::iterator findFirstVertex(INDEX_TYPE index);
	CListOfEdges::iterator findSecondVertex(INDEX_TYPE index);
};

