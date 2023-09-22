#pragma once

#include <vector>
#include "TCoord.h"

class __declspec(dllexport) CTIndex {
public:
	size_t a;
	size_t b;
	size_t c;

	CTIndex():a(0),b(0),c(0) {};
	CTIndex( size_t aa, size_t ab, size_t ac ):a(aa),b(ab),c(ac) {};
};

//class __declspec(dllexport) CArrayOfTIndex : public std::vector<CTIndex> {};
