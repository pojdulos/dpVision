#pragma once
#ifndef _VFACES_H_BY_DP_INCLUDED_
#define _VFACES_H_BY_DP_INCLUDED_

#include <vector>
#include "Face.h"

typedef std::multimap<size_t,size_t> Fmap;

class CvFaces : public std::vector<CFace>
{
private:
	Fmap fmap;

public:
	int fmap_erase( size_t key, size_t value )
	{
		int result = 0;
		std::pair<Fmap::iterator, Fmap::iterator> f_rng = fmap.equal_range( key );
		if ( f_rng.first != fmap.end() )
		{
			for (Fmap::iterator f_iter=f_rng.first; f_iter!=f_rng.second; )
			{
				if ( f_iter->second == value )
				{
					f_iter = fmap.erase( f_iter );
					++result;
				}
				else
				{
					++f_iter;
				}
			}
		}
		return result;
	};

	void push_back( const CFace & Val )
	{
		fmap.insert( Fmap::value_type( Val.A(), std::vector<CFace>::size() ) );
		fmap.insert( Fmap::value_type( Val.B(), std::vector<CFace>::size() ) );
		fmap.insert( Fmap::value_type( Val.C(), std::vector<CFace>::size() ) );

		std::vector<CFace>::push_back( Val );
	};

	
	CFace & operator[]( size_t index ) { return std::vector<CFace>::at( index ); }
	
	const CFace & operator[]( size_t index ) const { return std::vector<CFace>::at( index ); }
};

#endif /* _VFACES_H_BY_DP_INCLUDED_ */

/*
	CPMFfaces::iterator CPMFfaces::erase( CPMFfaces::iterator it )
	{
		fmap_erase( it->second.A(), it->first );
		fmap_erase( it->second.B(), it->first );
		fmap_erase( it->second.C(), it->first );

		return CPMFfaces::iterator( _faces::erase( _faces::iterator( it ) ) );
	};

	size_t CPMFfaces::erase( size_t key )
	{
		CPMFfaces::iterator it = _faces::find( key );

		if ( it == _faces::end() ) return 0;

		fmap_erase( it->second.A(), it->first );
		fmap_erase( it->second.B(), it->first );
		fmap_erase( it->second.C(), it->first );

		return _faces::erase( key );
	};

	int CPMFfaces::ChangeValue( size_t key, size_t oldval, size_t newval )
	{
		CPMFfaces::iterator it = _faces::find( key );
		
		if ( it == _faces::end() ) return -1;

		fmap_erase( oldval, key );
		fmap.insert( Fmap::value_type( newval, key ) );

		if ( oldval == it->second.A() )
		{
			it->second.A( newval );
			return 0;
		}
		else if ( oldval == it->second.B() )
		{
			it->second.B( newval );
			return 1;
		}
		if ( oldval == it->second.C() )
		{
			it->second.C( newval );
			return 2;
		}
		return -1;
	}

	CPMFfaces::iterator CPMFfaces::FindIfV( size_t v )
	{
		Fmap::iterator fit = fmap.find( v );

		if ( fit == fmap.end() ) return CPMFfaces::iterator( _faces::end() );

		_faces::iterator result = _faces::find( fit->second );
		return CPMFfaces::iterator( result );
	}

	void CPMFfaces::FindAllIfV( size_t v, std::vector<CPMFfaces::iterator> &ret )
	{
		ret.clear();

		Fmap::iterator fit = fmap.find( v );

		while ( ( fit != fmap.end() ) && ( v == (*fit).first ) )
		{
			ret.push_back( CPMFfaces::iterator ( _faces::iterator( _faces::find( fit->second ) ) ) );
			fit++;
		}
	}

	std::pair<CPMFfaces::iterator, bool> insert( _faces::value_type arg );
	CPMFfaces::iterator erase( CPMFfaces::iterator it );
	size_t erase( size_t key );
	
	int ChangeValue( size_t key, size_t oldval, size_t newval );
	CPMFfaces::iterator FindIfV( size_t v );
	void FindAllIfV( size_t v, std::vector<CPMFfaces::iterator> &ret );
*/
