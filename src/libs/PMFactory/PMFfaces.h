#pragma once
#ifndef _PMFACES_H_BY_DP_INCLUDED_
#define _PMFACES_H_BY_DP_INCLUDED_

#include <map>
#include "Face.h"

typedef std::map<size_t,CFace> _faces;
typedef std::multimap<INDEX_TYPE, size_t> Fmap;

class CPMFfaces : public _faces
{
private:
	Fmap fmap;

	int fmap_erase( INDEX_TYPE key, size_t value );

public:
	std::pair<CPMFfaces::iterator, bool> insert( _faces::value_type arg );
	CPMFfaces::iterator erase( CPMFfaces::iterator it );
	size_t erase( size_t key );
	
	int ChangeValue( size_t key, INDEX_TYPE oldval, INDEX_TYPE newval );
	CPMFfaces::iterator FindIfV( INDEX_TYPE v );
	void FindAllIfV( INDEX_TYPE v, std::vector<CPMFfaces::iterator> &ret );
};

#endif /* _PMFACES_H_BY_DP_INCLUDED_ */
