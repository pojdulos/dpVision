#include "StdAfx.h"
#include "BoundVector.h"

CBoundVector::CBoundVector( CPoint3d origin, CVector3d direction )
{
	first = origin;
	second = origin + direction;
}

CBoundVector::CBoundVector(CPoint3d p0, CPoint3d p1)
{
	first = p0;
	second = p1;
}

CBoundVector::~CBoundVector()
{
}
