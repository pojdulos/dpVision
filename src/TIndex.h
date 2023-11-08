#pragma once

#include <vector>
#include "TCoord.h"

class DPVISION_EXPORT CTIndex {
public:
	size_t a;
	size_t b;
	size_t c;

	CTIndex():a(0),b(0),c(0) {};
	CTIndex( size_t aa, size_t ab, size_t ac ):a(aa),b(ab),c(ac) {};
};

//class DPVISION_EXPORT CArrayOfTIndex : public std::vector<CTIndex> {};
