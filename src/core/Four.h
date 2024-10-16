#pragma once
#ifndef _FOUR_H_BY_DP_INCLUDED_
#define _FOUR_H_BY_DP_INCLUDED_

template<class _T> class Four
{
protected:
	_T x;
	_T y;
	_T z;
	_T w;

public:
	Four():x((_T)0),y((_T)0),z((_T)0),w((_T)0) {};
	Four( _T ax, _T ay, _T az, _T aw ):x(ax),y(ay),z(az),w(aw) {};
	Four( const Four<_T> &t ):x(t.x),y(t.y),z(t.z),w(t.w) {};

	~Four() {};
	
	inline void Set( _T ax, _T ay, _T az, _T aw ) { x = ax; y = ay; z = az; w = aw; };
	
	inline void SetIfBigger( _T ax, _T ay, _T az, _T aw ) {
		x = (ax>x)?ax:x; //MAX( x, ax );
		y = (ay>y)?ay:y;
		z = (az>z)?az:z;
		w = (aw>w)?aw:w;
	};

	inline void SetIfSmaller( _T ax, _T ay, _T az, _T aw ) {
		x = (ax<x)?ax:x; //MIN( x, ax );
		y = (ay<y)?ay:y;
		z = (az<z)?az:z;
		w = (aw<w)?aw:w;
	};

	// odczyt i zapis

	inline _T X() const { return x; };
	inline _T X( const _T ax ) { return x=ax; };

	inline _T Y() const { return y; };
	inline _T Y( const _T ay ) { return y=ay; };

	inline _T Z() const { return z; };
	inline _T Z( const _T az ) { return z=az; };

	inline _T W() const { return w; };
	inline _T W( const _T aw ) { return w=aw; };

	// OPERATORY
	
	// algebraiczne

	//------------------- operator przypisania ------------------------------------
	// wykomentowalem, bo przypisanie jest realizowane przez konstruktor kopiujacy
	//
	//	CTriple<_T> &operator=( const CTriple<_T>& c ) {
	//		return CTriple<_T>( x = c.x, y = c.y, z = c.z );
	//	};
	//-----------------------------------------------------------------------------
/*
	CTriple<_T> operator+( const CTriple<_T>& c ) const {
		return CTriple<_T>( x + c.x, y + c.y, z + c.z );
	};

	CTriple<_T> operator+=( const CTriple<_T>& c ) {
		return CTriple<_T>( x += c.x, y += c.y, z += c.z );
	};

	CTriple<_T> operator-( const CTriple<_T>& c ) const {
		return CTriple<_T>( x - c.x, y - c.y, z - c.z );
	};

	CTriple<_T> operator-=( const CTriple<_T>& c ) {
		return CTriple<_T>( x -= c.x, y -= c.y, z -= c.z );
	};

	CTriple<_T> operator/( const _T c ) const {
		return CTriple<_T>( x/c, y/c, z/c ); 
	};

	CTriple<_T> operator/=( const _T c ) const {
		return CTriple<_T>( x/=c, y/=c, z/=c ); 
	};

	CTriple<_T> operator%( const unsigned int c ) {
		return CTriple<_T>( x%c, y%c, z%c );
	};

	CTriple<_T> operator%=( const unsigned int c ) {
		return CTriple<_T>( x%=c, y%=c, z%=c );
	};

	// logiczne
*/
	bool operator==( const Four<_T>& c ) const {
		return ( x == c.x ) && ( y == c.y ) && ( z == c.z ) && ( w == c.w );
	};

	bool operator!=( const Four<_T>& c ) const {
		return ( x != c.x ) || ( y != c.y ) || ( z != c.z ) || ( w != c.w );
	};
/*
	bool operator<( const CTriple<_T> &c ) const {
		if ( x < c.x ) return true;
		else if ( x > c.x ) return false;
		else if ( y < c.y ) return true;
		else if ( y > c.y ) return false;
		return ( z < c.z );
	}

	bool operator<=( const CTriple<_T> &c ) const {
		if ( x < c.x ) return true;
		else if ( x > c.x ) return false;
		else if ( y < c.y ) return true;
		else if ( y > c.y ) return false;
		return ( z <= c.z );
	}

	bool operator>( const CTriple<_T> &c ) const {
		if ( x > c.x ) return true;
		else if ( x < c.x ) return false;
		else if ( x > c.y ) return true;
		else if ( x < c.y ) return false;
		return ( x > c.z );
	}

	bool operator>=( const CTriple<_T> &c ) const {
		if ( x > c.x ) return true;
		else if ( x < c.x ) return false;
		else if ( x > c.y ) return true;
		else if ( x < c.y ) return false;
		return ( x >= c.z );
	}
	*/
};

#endif /* _FOUR_H_BY_DP_INCLUDED_ */