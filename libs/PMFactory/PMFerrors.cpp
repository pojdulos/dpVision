#include "PMFerrors.h"

std::pair<CPMFerrors::iterator, bool> CPMFerrors::insert( _errors::value_type arg )
{
	std::pair<iterator, bool> result = _errors::insert( arg );
	if ( ! result.second ) return result;

	e1.insert( Emap1::value_type( arg.first.first, arg.first ) );
	e1.insert( Emap1::value_type( arg.first.second, arg.first ) );

	e2.insert( Emap2::value_type( arg.second, arg.first ) );

	return result;
};

int CPMFerrors::e1erase( size_t key, Pair value )
{
	int result = 0;
	std::pair<Emap1::iterator, Emap1::iterator> e1_rng = e1.equal_range( key );
	if ( e1_rng.first != e1.end() )
	{
		for (Emap1::iterator e1_iter=e1_rng.first; e1_iter!=e1_rng.second; )
		{
			if ( ( e1_iter->second.first == value.first ) && ( e1_iter->second.second == value.second ) )
			{
				e1_iter = e1.erase( e1_iter );
				++result;
			}
			else
			{
				++e1_iter;
			}
		}
	}
	return result;
}

int CPMFerrors::e2erase( error_type key, Pair value )
{
	int result = 0;

	std::pair<Emap2::iterator, Emap2::iterator> e2_rng = e2.equal_range( key );
	if ( e2_rng.first != e2.end() )
	{
		for (Emap2::iterator e2_iter=e2_rng.first; e2_iter!=e2_rng.second; )
		{
			if ( ( e2_iter->second.first == value.first ) && ( e2_iter->second.second == value.second ) )
			{
				e2_iter = e2.erase( e2_iter );
				++result;
			}
			else
			{
				++e2_iter;
			}
		}
	}
	return result;
};


CPMFerrors::iterator CPMFerrors::erase( CPMFerrors::iterator it )
{
	e2erase( it->second, it->first );

	e1erase( it->first.first, it->first );
	e1erase( it->first.second, it->first );

	return _errors::erase( _errors::iterator( it ) );
};

CPMFerrors::iterator CPMFerrors::GetMinError()
{
	_errors::iterator iter_min_error;
	Emap2::iterator e2_it = e2.begin();
	while ( (e2_it != e2.end()) && ( _errors::end() == ( iter_min_error = _errors::find( e2_it->second ) ) ) )
	{
		e2_it = e2.erase( e2_it );
	}

	return CPMFerrors::iterator( iter_min_error );
};

void CPMFerrors::ChangeValue( CPMFerrors::iterator it, error_type err )
{
	e2erase( it->second, it->first );
	it->second = err;
	e2.insert( Emap2::value_type( it->second, it->first ) );
}

CPMFerrors::iterator CPMFerrors::FindIfV( size_t v )
{
	Emap1::iterator eit = e1.find( v );

	if ( eit == e1.end() ) return CPMFerrors::iterator( _errors::end() );

	_errors::iterator result = _errors::find( eit->second );
	return CPMFerrors::iterator( result );
}

void CPMFerrors::FindAllIfV( size_t v, std::vector<CPMFerrors::iterator> &result )
{
	std::pair<Emap1::iterator, Emap1::iterator> e1_rng = e1.equal_range( v );
	
	if ( ( e1_rng.first == e1.end() ) || ( e1_rng.first == e1_rng.second ) ) return;

	result.clear();

	for ( Emap1::iterator eit=e1_rng.first; eit!=e1_rng.second; ++eit )
	{
		_errors::iterator iter = _errors::find( eit->second );

		if ( _errors::end() != iter )
		{
			result.push_back( iter );
		}
	}
	return;
}
