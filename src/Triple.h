#pragma once
#ifndef _TRIO_H_BY_DP_INCLUDED_
#define _TRIO_H_BY_DP_INCLUDED_

#include <Eigen/Geometry>
#include <type_traits>

template<
	typename _T,
	typename = typename std::enable_if<std::is_arithmetic<_T>::value, _T>::type >  class __declspec(dllexport) CTriple
{
public:
	union {
		_T v[3];
		struct { _T x, y, z; };
		struct { _T first, second, third; };
	};

public:
	CTriple()
	{
		static_assert(std::is_arithmetic<_T>::value, "CTriple<> type must be numeric");
		x = static_cast<_T>(0);
		y = static_cast<_T>(0);
		z = static_cast<_T>(0);
	};

	template<typename _W> CTriple( _W *f )
	{
		static_assert(std::is_arithmetic<_W>::value, "CTriple<> type must be numeric");
		x = static_cast<_T>(f[0]);
		y = static_cast<_T>(f[1]);
		z = static_cast<_T>(f[2]);
	};

	template<typename _W> CTriple(_W fx, _W fy, _W fz)
	{
		static_assert(std::is_arithmetic<_W>::value, "CTriple<> type must be numeric");
		x = static_cast<_T>(fx);
		y = static_cast<_T>(fy);
		z = static_cast<_T>(fz);
	};

	template<typename _W> CTriple(const CTriple<_W> &t)
	{
		static_assert(std::is_arithmetic<_W>::value, "CTriple<> type must be numeric");
		x = static_cast<_T>(t.X());
		y = static_cast<_T>(t.Y());
		z = static_cast<_T>(t.Z());
	};

	~CTriple() {};
	
	// export to Eigen::Vector3<Type>
	Eigen::Matrix<_T, 3, 1> toVector3()
	{
		Eigen::Matrix<_T, 3, 1> ev;
		ev(0, 0) = x;
		ev(1, 0) = y;
		ev(2, 0) = z;
		return ev;
	}

	// export to Eigen::Vector4<Type>
	Eigen::Matrix<_T, 4, 1> toVector4()
	{
		Eigen::Matrix<_T, 4, 1> ev;
		ev(0, 0) = x;
		ev(1, 0) = y;
		ev(2, 0) = z;
		ev(3, 0) = 1;
		return ev;
	}

	// export to Eigen::RowVector3<Type>
	Eigen::Matrix<_T, 1, 3> toRowVector3()
	{
		Eigen::Matrix<_T, 1, 3> erv;
		erv(0, 0) = x;		erv(0, 1) = y;		erv(0, 2) = z;
		return erv;
	}

	// export to Eigen::RowVector4<Type>
	Eigen::Matrix<_T, 1, 4> toRowVector4()
	{
		Eigen::Matrix<_T, 1, 4> erv;
		erv(0, 0) = x;		erv(0, 1) = y;		erv(0, 2) = z;		erv(0, 3) = 1;
		return erv;
	}


	template<typename _W> inline void Set( CTriple<_W> t )
	{
		static_assert(std::is_arithmetic<_W>::value, "CTriple<> type must be numeric");
		x = static_cast<_T>(t.X());
		y = static_cast<_T>(t.Y());
		z = static_cast<_T>(t.Z());
	};

	template<typename _W> inline void Set( _W w[3] )
	{
		static_assert(std::is_arithmetic<_W>::value, "Data type must be numeric");
		x = static_cast<_T>(w[0]);
		y = static_cast<_T>(w[1]);
		z = static_cast<_T>(w[2]);
	};

	template<typename _W> inline void Set(_W ax, _W ay, _W az)
	{
		static_assert(std::is_arithmetic<_W>::value, "Data type must be numeric");
		x = static_cast<_T>(ax);
		y = static_cast<_T>(ay);
		z = static_cast<_T>(az);
	};

	inline void SetIfBigger( _T ax, _T ay, _T az ) {
		x = (ax>x)?ax:x; //MAX( x, ax );
		y = (ay>y)?ay:y;
		z = (az>z)?az:z;
	};

	template<typename _W> inline void SetIfBigger( CTriple<_W> t ) {
		static_assert(std::is_arithmetic<_W>::value, "Data type must be numeric");
		x = (t.x>x) ? t.x : x; //MAX( x, ax );
		y = (t.y>y) ? t.y : y;
		z = (t.z>z) ? t.z : z;
	};

	template<typename _W> inline void SetIfBigger( _W t[3] ) {
		static_assert(std::is_arithmetic<_W>::value, "Data type must be numeric");
		x = (t[0]>x) ? t[0] : x; //MAX( x, ax );
		y = (t[1]>y) ? t[1] : y;
		z = (t[2]>z) ? t[2] : z;
	};

	inline void SetIfSmaller( _T ax, _T ay, _T az ) {
		x = (ax<x)?ax:x; //MIN( x, ax );
		y = (ay<y)?ay:y;
		z = (az<z)?az:z;
	};

	template<typename _W> inline void SetIfSmaller( CTriple<_W> t ) {
		static_assert(std::is_arithmetic<_W>::value, "Data type must be numeric");
		x = (t.x<x) ? t.x : x; //MIN( x, ax );
		y = (t.y<y) ? t.y : y;
		z = (t.z<z) ? t.z : z;
	};

	template<typename _W> inline void SetIfSmaller( _W t[3] ) {
		static_assert(std::is_arithmetic<_W>::value, "Data type must be numeric");
		x = (t[0]<x) ? t[0] : x; //MIN( x, ax );
		y = (t[1]<y) ? t[1] : y;
		z = (t[2]<z) ? t[2] : z;
	};


	template<typename _W> inline void scaleByVector(CTriple<_W> t)
	{
		static_assert(std::is_arithmetic<_W>::value, "CTriple<> type must be numeric");
		x *= static_cast<_T>(t.x);
		y *= static_cast<_T>(t.y);
		z *= static_cast<_T>(t.z);
	};



	// odczyt i zapis


	inline _T * toVector() { return &v[0]; };

	inline _T X() const { return x; };
	inline _T X( const _T ax ) { return x=ax; };


	inline _T Y() const { return y; };
	inline _T Y( const _T ay ) { return y=ay; };

	inline _T Z() const { return z; };
	inline _T Z( const _T az ) { return z=az; };

	template<typename _W> _W X() const { return static_cast<_W>(x); };
	template<typename _W> _W Y() const { return static_cast<_W>(y); };
	template<typename _W> _W Z() const { return static_cast<_W>(z); };

	// OPERATORY
	
	_T& operator[](size_t index) { return v[index]; }

	const _T& operator[](size_t index) const { return v[index]; }

	// algebraiczne

	//------------------- operator przypisania ------------------------------------
	// wykomentowalem, bo przypisanie jest realizowane przez konstruktor kopiujacy
	//
	//	CTriple<_T> &operator=( const CTriple<_T>& c ) {
	//		return CTriple<_T>( x = c.x, y = c.y, z = c.z );
	//	};
	//-----------------------------------------------------------------------------

	CTriple<_T> operator-() const {
		return CTriple<_T>(-x, -y, -z);
	};

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

	CTriple<_T> operator*(const _T c) const {
		return CTriple<_T>(x * c, y * c, z * c);
	};

	CTriple<_T> operator/=( const _T c ) {
		return CTriple<_T>( x/=c, y/=c, z/=c ); 
	};

	CTriple<_T> operator*=(const _T c) {
		return CTriple<_T>(x *= c, y *= c, z *= c);
	};


	// logiczne

	bool operator==( const CTriple<_T>& c ) const {
		return ( x == c.x ) && ( y == c.y ) && ( z == c.z );
	};

	bool operator!=( const CTriple<_T>& c ) const {
		return ( x != c.x ) || ( y != c.y ) || ( z != c.z );
	};

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


};


#endif /* _TRIO_H_BY_DP_INCLUDED_ */