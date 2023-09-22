#pragma once
#ifndef XKDTREEBUFFER_H
#define XKDTREEBUFFER_H

#include <map>
#include <vector>

#include "Global.h"

#include "XVector.h"


struct comp
{
	template<typename T>
	bool operator()(const T& l, const T& r) const
	{
		return l.first > r.first || (l.first == r.first && l.second > r.second);
	}
};


template< class RandomIt >
void sortHeapX(RandomIt first, RandomIt last)
{
	while (first != last)
		std::pop_heap(first, last--);
}


template<typename _Typ, int _Dim>
	class XKDTreeBuffer
{
public:
	typedef std::vector<std::pair<_Typ, INDEX_TYPE>> XKDTreeResult;

	XVector<_Typ, _Dim>* dataPtr;
	int dataSize;

	XVector<_Typ, _Dim> pattern, rayDir;
	int searchLimit;

	_Typ lastGood_d;
	_Typ lastGood_d2;
	_Typ maxdist;
	_Typ maxdist2;

	XKDTreeResult results;
private:
};

#endif
