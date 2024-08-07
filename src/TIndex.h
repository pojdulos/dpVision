#pragma once

#include <vector>
#include "TCoord.h"

class DPVISION_EXPORT CTIndex {
public:
	union {
		INDEX_TYPE v[3];
		struct { INDEX_TYPE a, b, c; };
	};

	CTIndex():a(0),b(0),c(0) {};
	CTIndex( size_t aa, size_t ab, size_t ac ):a(aa),b(ab),c(ac) {};

	INDEX_TYPE& operator[](const unsigned int idx) {
		assert((idx < 3) && "Index out of range");
		return v[idx];
	}

	const INDEX_TYPE& operator[](const unsigned int idx) const {
		assert((idx < 3) && "Index out of range");
		return v[idx];
	}
};

//class DPVISION_EXPORT CArrayOfTIndex : public std::vector<CTIndex> {};
