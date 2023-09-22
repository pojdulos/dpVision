#pragma once
#ifndef _MATRIX3X3_H_BY_DP_INCLUDED_
#define _MATRIX3X3_H_BY_DP_INCLUDED_
#include "Triple.h"

template<class _T> class CMatrix3x3
{
	// tak reprezentujê macierz:
	// a1  a2  a3
	// a4  a5  a6
	// a7  a8  a9
	_T a1; _T a2; _T a3;
	_T a4; _T a5; _T a6;
	_T a7; _T a8; _T a9;

public:
	CMatrix3x3()
	{
		a1 = a2 = a3 = a4 = a5 = a6 = a7 = a8 = a9 = 0;
	}

	CMatrix3x3( _T p1, _T p2, _T p3, _T p4, _T p5, _T p6, _T p7, _T p8, _T p9 )
	{
		a1 = p1; a2 = p2; a3 = p3;
		a4 = p4; a5 = p5; a6 = p6;
		a7 = p7; a8 = p8; a9 = p9;
	}

	// wyznacznik macierzy
	inline const _T det()
	{
		return ( a1*a5*a9 + a2*a6*a7 + a3*a4*a8 - a3*a5*a7 - a2*a4*a9 - a1*a6*a8 );
	}

	// transpozycja
	inline const CMatrix3x3<_T> T()
	{
		return CMatrix3x3<_T>( a1, a4, a7, a2, a5, a8, a3, a6, a9 );
	}
};

#endif /* _MATRIX3X3_H_BY_DP_INCLUDED_ */