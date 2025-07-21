#pragma once
#ifndef _UTILITIES_H_BY_DP_INCLUDED_
#define _UTILITIES_H_BY_DP_INCLUDED_

#include "dll_global.h"

#include "Global.h"
#include <algorithm>

template<typename _T> _T MIN( const _T a, const _T b) { return (a<b)?a:b; };
template<typename _T> _T MAX( const _T a, const _T b) { return (a>b)?a:b; };

template<typename _T> _T MIN3( const _T a, const _T b, const _T c) { return MIN( MIN( a, b ), c ); };
template<typename _T> _T MAX3( const _T a, const _T b, const _T c) { return MAX( MAX( a, b ), c ); };

template<typename _T> _T MIN4( const _T a, const _T b, const _T c, const _T d ) { return MIN( MIN( a, b ), MIN( c, d ) ); };
template<typename _T> _T MAX4( const _T a, const _T b, const _T c,const  _T d ) { return MAX( MAX( a, b ), MAX( c, d ) ); };

#ifndef ALMOST_NULL
#define ALMOST_NULL 0.000001
#endif

inline bool isAlmostNull(double f) { return abs(f) <= ALMOST_NULL; }
inline bool isAlmostOne(double f) { return abs(1.0 - abs(f)) <= ALMOST_NULL; }

DPVISION_EXPORT double smartRound(double d, int decimal);

#ifndef deg2rad
//#define deg2rad(a) ((a)*PI/180.0)
#define deg2rad(x) ((x) * 0.01745329251994329576923690768489)
#endif

#ifndef rad2deg
//#define rad2deg(a) ((a)*180.0/PI)
#define rad2deg(x) ((x) * 57.295779513082320876798154814105)
#endif

template<typename _W> _W angle180(const _W x) {
	while (x > 180) x -= 360;
	while (x <= -180) x += 360;
	return x;
};

template<typename _W> _W angle360(const _W x) {
	while (x >= 360) x -= 360;
	while (x < 0) x += 360;
	return x;
};

#endif /* _UTILITIES_H_BY_DP_INCLUDED_ */