#pragma once

#include <vector>

class __declspec(dllexport) CTCoord {
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

class __declspec(dllexport) CArrayOfTCoord : public std::vector<CTCoord> {};

