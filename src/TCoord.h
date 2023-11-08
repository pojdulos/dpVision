#pragma once

#include <vector>

class DPVISION_EXPORT CTCoord {
public:
	union {
		float arr[2];
		struct { float s, t; };
		struct { float first, second; };
	};

	CTCoord():s(0.0f),t(0.0f) {};
	CTCoord( float as, float at ):s(as),t(at) {};

	float& operator[](std::size_t index) {
		return arr[index];
	}

	const float& operator[](std::size_t index) const {
		return arr[index];
	}
};

class DPVISION_EXPORT CArrayOfTCoord : public std::vector<CTCoord> {};

