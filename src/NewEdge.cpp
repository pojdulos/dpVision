#include "NewEdge.h"



// Uwaga: z�o�ono�� O(n)
MapOfNewEdges::iterator MapOfNewEdges::findFirstVertex(unsigned int index)
{
	if (this->empty()) return this->end();

	MapOfNewEdges::iterator it = this->begin();
	while ((*it).first.first != index) {
		it++;

		if (it == this->end()) return it;
	}

	return it;
}

// Uwaga: z�o�ono�� O(n)
MapOfNewEdges::iterator MapOfNewEdges::findSecondVertex(unsigned int index)
{
	if (this->empty()) return this->end();

	MapOfNewEdges::iterator it = this->begin();
	while ((*it).first.second != index) {
		it++;

		if (it == this->end()) return it;
	}

	return it;
}

// Uwaga: z�o�ono�� O(n)
MapOfNewEdges::iterator MapOfNewEdges::findVertex(unsigned int index)
{
	if (this->empty()) return this->end();

	MapOfNewEdges::iterator it = this->begin();

	NewEdge e = NewEdge( *it );

	while (! e.hasVertex(index) )
	{
		it++;

		if (it == this->end()) return it;

		e = NewEdge(*it);
	}

	return it;
}


// Uwaga: z�o�ono�� O(n)
MapOfNewEdges::iterator MapOfNewEdges::findAnyOfVertices(unsigned int index1, unsigned int index2)
{
	if (this->empty()) return this->end();

	MapOfNewEdges::iterator it = this->begin();

	NewEdge e( *it );

	while (!e.hasOneOfVertices(index1, index2))
	{
		it++;

		if (it == this->end()) return it;

		e = NewEdge(*it);
	}

	return it;
}


MapOfNewEdges::iterator MapOfNewEdges::find(unsigned int f, unsigned int s)
{
	MapOfNewEdges::iterator found = std::map< NewEdge::first_type, NewEdge::second_type >::find( NewEdge::first_type(f, s) );

	if (found == this->end())
	{
		found = std::map< NewEdge::first_type, NewEdge::second_type >::find( NewEdge::first_type(s, f) );
	}

	return found;
}

