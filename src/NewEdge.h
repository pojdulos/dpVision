#pragma once

#include "dll_global.h"

#include <set>
#include <map>


class MapOfNewEdges;
//klasa testowa, uprasza sie o nieu¿ywanie
class DPVISION_EXPORT NewEdge : public std::pair< std::pair<unsigned int, unsigned int>, std::set<unsigned int> >
{
public:
	NewEdge()
	{
		first.first = 0;
		first.second = 0;
	}

	NewEdge(unsigned int f, unsigned int s)
	{
			first.first = f;
			first.second = s;
	};

	NewEdge(unsigned int f, unsigned int s, unsigned int t)
	{
		first.first = f;
		first.second = s;
		second.insert(t);
	};

	NewEdge(const std::pair< NewEdge::first_type, NewEdge::second_type > e)
	{
		first.first = e.first.first;
		first.second = e.first.second;
		second = e.second;
	}

	NewEdge(const NewEdge::first_type& f, NewEdge::second_type& s)
	{
		first.first = f.first;
		first.second = f.second;
		second = s;
	};

	NewEdge(const NewEdge& e)
	{
		first.first = e.first.first;
		first.second = e.first.second;
		second = e.second;
	};

	void invert() {
		unsigned int tmp = first.first;
		first.first = first.second;
		first.second = tmp;
	}

	inline unsigned int v1() { return first.first; }
	inline unsigned int v2() { return first.second; }

	inline bool isInvalid() const { return first.first == first.second; }
	inline bool isValid() const { return first.first != first.second; }
	
	inline bool hasFace() const { return ! second.empty(); }

	inline bool isBoundary() const { return isValid() && (second.size() == 1); }

	inline const bool hasVertex(unsigned int v) const { return (v == first.first) || (v == first.second); }
	inline bool hasOneOfVertices(unsigned int v1, unsigned int v2) const { return (v1 == first.first) || (v1 == first.second) || (v2 == first.first) || (v2 == first.second); }
};

class DPVISION_EXPORT  MapOfNewEdges : public std::map< NewEdge::first_type, NewEdge::second_type >
{
public:
	MapOfNewEdges::iterator findFirstVertex(unsigned int index);
	MapOfNewEdges::iterator findSecondVertex(unsigned int index);

	MapOfNewEdges::iterator findVertex(unsigned int index);

	MapOfNewEdges::iterator findAnyOfVertices(unsigned int index1, unsigned int index2);

	MapOfNewEdges::iterator find(unsigned int f, unsigned int s);
};
