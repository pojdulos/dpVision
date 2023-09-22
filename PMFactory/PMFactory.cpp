#include "PMFactory.h"

PMFactory::PMFactory(void)
{
	mSolidPoints.clear();
};

PMFactory::~PMFactory(void)
{
};


bool PMFactory::isSolid( CPoint3f pt )
{
	for ( _setOfPoints::iterator it = mSolidPoints.begin(); it != mSolidPoints.end(); it++ )
	{
		if ( ( pt.X() == it->X() ) && ( pt.Y() == it->Y() ) && ( pt.Z() == it->Z() ) )
		{
			return true;
		}
	}

	return false;
}