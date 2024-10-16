#pragma once

#include "Point3.h"
#include "Vector3.h"

class  DPVISION_EXPORT  CBoundVector	: public std::pair<CPoint3d,CPoint3d>
{

public:
	CBoundVector() { first = CPoint3d(); second = CPoint3d(); };
	CBoundVector( CPoint3d origin, CVector3d direction );
	CBoundVector( CPoint3d p0, CPoint3d p1 );
	~CBoundVector();

	CPoint3d origin() const { return first; };
	CVector3d direction() const { return CVector3d(first, second); };

	CPoint3d & p0() { return first; };
	CPoint3d & p1() { return second; };

	void normalize() { second = first + CVector3d(first, second).getNormalized(); };
	CBoundVector getNormalized() { return CBoundVector(first, first + CVector3d(first, second).getNormalized()); };

	double length() { return CVector3d(first, second).length(); };
	double squaredLength() { return CVector3d(first, second).squaredLength(); };

	void invert() { second = first - CVector3d(first, second); };
	CBoundVector operator-() const { return CBoundVector( first, CVector3d(second, first) ); };
};

