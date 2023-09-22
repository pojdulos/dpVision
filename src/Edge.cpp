#include "Global.h"
#include "Edge.h"

// Uwaga: z³o¿onoœæ O(n)
CSetOfEdges::iterator CSetOfEdges::findFirstVertex(INDEX_TYPE index)
{
	if (this->empty()) return this->end();

	CSetOfEdges::iterator it = this->begin();
	while ((*it).first != index) {
		it++;

		if (it == this->end()) return it;
	}

	return it;
}

// Uwaga: z³o¿onoœæ O(n)
CSetOfEdges::iterator CSetOfEdges::findSecondVertex(INDEX_TYPE index)
{
	if (this->empty()) return this->end();

	CSetOfEdges::iterator it = this->begin();
	while ((*it).second != index) {
		it++;

		if (it == this->end()) return it;
	}

	return it;
}

CSetOfEdges::iterator CSetOfEdges::find(INDEX_TYPE f, INDEX_TYPE s)
{
	//if (f < s)
	//{
		return std::set<CEdge>::find( CEdge(f, s) );
	//}
	//else
	//{
	//	return std::set<CEdge>::find(CEdge(s, f));
	//}
}

CSetOfEdges::iterator CSetOfEdges::find( CEdge e)
{
	//if (e.first < e.second)
	//{
		return std::set<CEdge>::find(CEdge(e.first, e.second));
	//}
	//else
	//{
	//	return std::set<CEdge>::find(CEdge(e.second, e.first));
	//}
}



// Uwaga: z³o¿onoœæ O(n)
CListOfEdges::iterator CListOfEdges::findFirstVertex(INDEX_TYPE index)
{
	if (this->empty()) return this->end();

	CListOfEdges::iterator it = this->begin();
	while ((*it).first != index) {
		it++;

		if (it == this->end()) return it;
	}

	return it;
}

// Uwaga: z³o¿onoœæ O(n)
CListOfEdges::iterator CListOfEdges::findSecondVertex(INDEX_TYPE index)
{
	if (this->empty()) return this->end();

	CListOfEdges::iterator it = this->begin();
	while ((*it).second != index) {
		it++;

		if (it == this->end()) return it;
	}

	return it;
}

void CEdge::addFace(INDEX_TYPE idx)
{
	this->faces.insert(idx);
}
