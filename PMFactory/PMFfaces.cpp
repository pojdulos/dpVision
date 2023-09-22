#include "PMFfaces.h"

std::pair<CPMFfaces::iterator, bool> CPMFfaces::insert( _faces::value_type arg )
{
	std::pair<iterator, bool> result = _faces::insert( arg );
	if ( ! result.second ) return result;

	fmap.insert( Fmap::value_type( arg.second.A(), arg.first ) );
	fmap.insert( Fmap::value_type( arg.second.B(), arg.first ) );
	fmap.insert( Fmap::value_type( arg.second.C(), arg.first ) );

	return result;
};

int CPMFfaces::fmap_erase( size_t key, size_t value )
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
