#pragma once
#ifndef _FACE4_H_BY_DP_INCLUDED_
#define _FACE4_H_BY_DP_INCLUDED_

#include "Four.h"
//#include "Utilities.h"
#include "Wektor3D.h"
#include "Vertex.h"

class CFace4 : public Four<unsigned int> {
public:
	CFace4() { x=0; y=0; z=0; w=0; };
	CFace4( unsigned int ax, unsigned int ay, unsigned int az, unsigned int aw ) { x=ax; y=ay; z=az; w=aw; };
	
	inline unsigned int A() const { return x; };
	inline unsigned int A( const unsigned int ax ) { return x=ax; };

	inline unsigned int B() const { return y; };
	inline unsigned int B( const unsigned int ay ) { return y=ay; };

	inline unsigned int C() const { return z; };
	inline unsigned int C( const unsigned int az ) { return z=az; };

	inline unsigned int D() const { return w; };
	inline unsigned int D( const unsigned int aw ) { return w=aw; };

	CVector3f GetNormal( const std::vector<CVertex> &vertices )
	{
		CVector3f v1, v2;
		CVector3f vn;

		if ( MAX3( x, y, z ) < vertices.size() )
		{
			// wektor f.A->f.B
			v1 = vertices[y] - vertices[x];

			// wektor f.B->f.C
			v2 = vertices[z] - vertices[y];
			
			// UWAGA: iloczyn wektorowy !
			vn = v1 * v2;

			// Normalizacja
			vn.Normalize();
		}

		return ( vn );
	}

	int HasVertex( const unsigned int v_id )
	{
		if (x==v_id) return 0;
		else if (y==v_id) return 1;
		else if (z==v_id) return 2;
		else if (w==v_id) return 3;

		return -1;
	}

	void SetAt( int n, const unsigned int v_id )
	{
		if (n==0) x=v_id;
		else if (n==1) y=v_id;
		else if (n==2) z=v_id;
		else if (n==3) w=v_id;
	}
};

class CArrayOfFaces4 : public std::vector<CFace4>	{};
typedef CArrayOfFaces4 _vof4;

#endif /* _FACE4_H_BY_DP_INCLUDED_ */