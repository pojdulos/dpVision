#pragma once
#ifndef _PMFERRORS_H_BY_DP_INCLUDED_
#define _PMFERRORS_H_BY_DP_INCLUDED_

#include <map>
#include <vector>
#include "matrix.h"

typedef float error_type;

typedef std::pair<size_t,size_t> Pair;

typedef std::multimap<size_t,Pair> Emap1;
typedef std::multimap<error_type,Pair> Emap2;

typedef std::map<size_t,Matrix> Matrices;

typedef std::map<Pair, error_type> _errors;

class CPMFerrors : public _errors {
	Emap1 e1;
	Emap2 e2;

	int e2erase( error_type key, Pair value );
	int e1erase( size_t key, Pair value );

public:
	std::pair<CPMFerrors::iterator, bool> insert( _errors::value_type arg );
	CPMFerrors::iterator erase( CPMFerrors::iterator it );
	void ChangeValue( CPMFerrors::iterator it, error_type err );

	CPMFerrors::iterator GetMinError();
	CPMFerrors::iterator FindIfV( size_t v );
	void FindAllIfV( size_t v, std::vector<CPMFerrors::iterator> &result );
};

#endif /* _PMFERRORS_H_BY_DP_INCLUDED_ */