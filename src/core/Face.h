#pragma once

#include "Global.h"

#include "Utilities.h"

#include "Vector3.h"
#include "Point3.h"
#include "Vertex.h"

class DPVISION_EXPORT CFace
{
   INDEX_TYPE x, y, z;

public:
	CFace() { x=0; y=0; z=0; };
	CFace(INDEX_TYPE ax, INDEX_TYPE ay, INDEX_TYPE az ) { x=ax; y=ay; z=az; };
	
	inline INDEX_TYPE A() const { return x; };
	inline INDEX_TYPE A( const INDEX_TYPE _a ) { return x=_a; };

	inline INDEX_TYPE B() const { return y; };
	inline INDEX_TYPE B( const INDEX_TYPE _b ) { return y=_b; };

	inline INDEX_TYPE C() const { return z; };
	inline INDEX_TYPE C( const INDEX_TYPE _c ) { return z=_c; };

	inline void Set(INDEX_TYPE ax, INDEX_TYPE ay, INDEX_TYPE az)
	{
		x = ax;
		y = ay;
		z = az;
	}
	
	void invert();

	CVector3d getNormal(const std::vector<CVertex> &vertices);

	static CVector3d triangleNormal(CPoint3d A, CPoint3d B, CPoint3d C );
	static double triangleArea(CPoint3d n0, CPoint3d n1, CPoint3d n2);

	int hasVertex(const INDEX_TYPE v_id);
	void setAt(int n, const INDEX_TYPE v_id);
    INDEX_TYPE at(int n);

	bool operator<( const CFace &c ) const {
		if ( x < c.x ) return true;
		else if ( x > c.x ) return false;
		else if ( y < c.y ) return true;
		else if ( y > c.y ) return false;
		return ( z < c.z );
	}
};
