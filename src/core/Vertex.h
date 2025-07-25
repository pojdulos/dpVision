#pragma once
#ifndef _VERTEX_H_BY_DP_INCLUDED_
#define _VERTEX_H_BY_DP_INCLUDED_

#include "dll_global.h"

#include <map>
#include <vector>
#include "Point3.h"
#include "RGBA.h"

#ifndef VERTEX_INTERNAL_TYPE
#define VERTEX_INTERNAL_TYPE float
#endif /* VERTEX_INTERNAL_TYPE */

class DPVISION_EXPORT CVertex : public CPoint3<VERTEX_INTERNAL_TYPE> {
public:
	typedef VERTEX_INTERNAL_TYPE Type;
	CVertex() :CPoint3<VERTEX_INTERNAL_TYPE>() {};
	template<typename _W> CVertex(_W fx, _W fy, _W fz) :CPoint3<VERTEX_INTERNAL_TYPE>(fx, fy, fz) {};
	template<typename _W> CVertex(const CTriple<_W>& t) :CPoint3<VERTEX_INTERNAL_TYPE>(t) {};

	//static inline void render( CVertex &v )	{ glVertex3fv(v.toVector()); };
	//static inline void render( CVertex &v, CRGBA &c ) { glColor4ubv(c.V()); glVertex3fv(v.toVector()); };
};

class DPVISION_EXPORT _mapOfVertices : public std::map<size_t, size_t> {};

#endif /* _VERTEX_H_BY_DP_INCLUDED_ */