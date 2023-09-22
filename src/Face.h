#pragma once
#ifndef _FACE_H_BY_DP_INCLUDED_
#define _FACE_H_BY_DP_INCLUDED_

#include "Global.h"
#include "Triple.h"

#include "Utilities.h"
#include "Vector3.h"
#include "Point3.h"
#include "Vertex.h"

class __declspec(dllexport) CFace : public CTriple<INDEX_TYPE>
{
public:
	CFace() { x=0; y=0; z=0; };
	CFace(INDEX_TYPE ax, INDEX_TYPE ay, INDEX_TYPE az ) { x=ax; y=ay; z=az; };
	
	inline INDEX_TYPE A() const { return x; };
	inline INDEX_TYPE A( const INDEX_TYPE _a ) { return x=_a; };

	inline INDEX_TYPE B() const { return y; };
	inline INDEX_TYPE B( const INDEX_TYPE _b ) { return y=_b; };

	inline INDEX_TYPE C() const { return z; };
	inline INDEX_TYPE C( const INDEX_TYPE _c ) { return z=_c; };

	void invert();

	CVector3d getNormal(const std::vector<CVertex> &vertices);

	static CVector3d triangleNormal(CPoint3d A, CPoint3d B, CPoint3d C );
	static double triangleArea(CPoint3d n0, CPoint3d n1, CPoint3d n2);

	int hasVertex(const INDEX_TYPE v_id);
	void setAt(int n, const INDEX_TYPE v_id);
};

//class __declspec(dllexport) CArrayOfFaces : public std::vector<CFace>	{};
//typedef std::vector<CFace> _vof;

#endif /* _FACE_H_BY_DP_INCLUDED_ */