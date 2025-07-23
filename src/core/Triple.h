#pragma once
#ifndef _TRIO_H_BY_DP_INCLUDED_
#define _TRIO_H_BY_DP_INCLUDED_

#include "dll_global.h"

#include <Eigen/Geometry>
#include <type_traits>
#include <cassert>

/**
 * @brief Template class representing a 3D vector or point.
 * 
 * Provides storage and operations for three numeric values of any arithmetic type.
 * Includes convenient accessors, mutators, and arithmetic/logical operators.
 * 
 * @tparam _T Numeric type (e.g., float, double, int)
 */
template<
	typename _T,
	typename = typename std::enable_if<std::is_arithmetic<_T>::value, _T>::type >
class DPVISION_EXPORT CTriple
{
	static_assert(std::is_arithmetic<_T>::value, "CTriple<> type must be numeric");
public:
	/**
	 * @brief Access to coordinates via different aliases.
	 * 
	 * v[0] == x == first
	 * v[1] == y == second
	 * v[2] == z == third
	 */
	union {
		_T v[3];                ///< Array of coordinates
		struct { _T x, y, z; }; ///< Cartesian coordinates
		struct { _T first, second, third; }; ///< Alternative names
	};

public:
	/**
	 * @brief Default constructor. Initializes all coordinates to 0.
	 */
	constexpr CTriple() noexcept
		: x(static_cast<_T>(0)), y(static_cast<_T>(0)), z(static_cast<_T>(0)) {}

	/**
	 * @brief Constructor from pointer to array of 3 values.
	 * @param f Pointer to array of 3 numeric values.
	 */
	template<typename _W> explicit CTriple(const _W* f)
	{
		static_assert(std::is_arithmetic<_W>::value, "CTriple<> type must be numeric");
		x = static_cast<_T>(f[0]);
		y = static_cast<_T>(f[1]);
		z = static_cast<_T>(f[2]);
	}

	/**
	 * @brief Constructor from three values.
	 * @param fx Value for x
	 * @param fy Value for y
	 * @param fz Value for z
	 */
	template<typename _W> CTriple(_W fx, _W fy, _W fz)
	{
		static_assert(std::is_arithmetic<_W>::value, "CTriple<> type must be numeric");
		x = static_cast<_T>(fx);
		y = static_cast<_T>(fy);
		z = static_cast<_T>(fz);
	}

	/**
	 * @brief Copy constructor from another CTriple of possibly different type.
	 * @param t Another CTriple object
	 */
	template<typename _W> CTriple(const CTriple<_W> &t)
	{
		static_assert(std::is_arithmetic<_W>::value, "CTriple<> type must be numeric");
		x = static_cast<_T>(t.X());
		y = static_cast<_T>(t.Y());
		z = static_cast<_T>(t.Z());
	}

	/// Default destructor
	~CTriple() = default;

	/**
	 * @brief Convert to Eigen::Vector3 (column vector).
	 * @return Eigen 3x1 vector
	 */
	Eigen::Matrix<_T, 3, 1> toVector3() const
	{
		Eigen::Matrix<_T, 3, 1> ev;
		ev(0, 0) = x;
		ev(1, 0) = y;
		ev(2, 0) = z;
		return ev;
	}

	/**
	 * @brief Convert to Eigen::RowVector3 (row vector).
	 * @return Eigen 1x3 vector
	 */
	Eigen::Matrix<_T, 1, 3> toRowVector3() const
	{
		Eigen::Matrix<_T, 1, 3> erv;
		erv(0, 0) = x;		erv(0, 1) = y;		erv(0, 2) = z;
		return erv;
	}

	/**
	 * @brief Set values from another CTriple.
	 * @param t Another CTriple object
	 */
	template<typename _W> inline void Set(const CTriple<_W>& t)
	{
		static_assert(std::is_arithmetic<_W>::value, "CTriple<> type must be numeric");
		x = static_cast<_T>(t.X());
		y = static_cast<_T>(t.Y());
		z = static_cast<_T>(t.Z());
	}

	/**
	 * @brief Set values from an array.
	 * @param w Array of 3 values
	 */
	template<typename _W> inline void Set(const _W w[3])
	{
		static_assert(std::is_arithmetic<_W>::value, "Data type must be numeric");
		x = static_cast<_T>(w[0]);
		y = static_cast<_T>(w[1]);
		z = static_cast<_T>(w[2]);
	}

	/**
	 * @brief Set values directly.
	 * @param ax Value for x
	 * @param ay Value for y
	 * @param az Value for z
	 */
	template<typename _W> inline void Set(_W ax, _W ay, _W az)
	{
		static_assert(std::is_arithmetic<_W>::value, "Data type must be numeric");
		x = static_cast<_T>(ax);
		y = static_cast<_T>(ay);
		z = static_cast<_T>(az);
	}

	/**
	 * @brief Set coordinates only if new values are greater.
	 * @param ax, ay, az New values
	 */
	inline void SetIfBigger( _T ax, _T ay, _T az ) {
		x = (ax>x)?ax:x;
		y = (ay>y)?ay:y;
		z = (az>z)?az:z;
	}

	/**
	 * @brief Set coordinates only if new values are greater (from another CTriple).
	 * @param t Another CTriple object
	 */
	template<typename _W> inline void SetIfBigger( CTriple<_W> t ) {
		static_assert(std::is_arithmetic<_W>::value, "Data type must be numeric");
		x = (t.x>x) ? t.x : x;
		y = (t.y>y) ? t.y : y;
		z = (t.z>z) ? t.z : z;
	}

	/**
	 * @brief Set coordinates only if new values are greater (from array).
	 * @param t Array of 3 values
	 */
	template<typename _W> inline void SetIfBigger( _W t[3] ) {
		static_assert(std::is_arithmetic<_W>::value, "Data type must be numeric");
		x = (t[0]>x) ? t[0] : x;
		y = (t[1]>y) ? t[1] : y;
		z = (t[2]>z) ? t[2] : z;
	}

	/**
	 * @brief Set coordinates only if new values are smaller.
	 * @param ax, ay, az New values
	 */
	inline void SetIfSmaller( _T ax, _T ay, _T az ) {
		x = (ax<x)?ax:x;
		y = (ay<y)?ay:y;
		z = (az<z)?az:z;
	}

	/**
	 * @brief Set coordinates only if new values are smaller (from another CTriple).
	 * @param t Another CTriple object
	 */
	template<typename _W> inline void SetIfSmaller( CTriple<_W> t ) {
		static_assert(std::is_arithmetic<_W>::value, "Data type must be numeric");
		x = (t.x<x) ? t.x : x;
		y = (t.y<y) ? t.y : y;
		z = (t.z<z) ? t.z : z;
	}

	/**
	 * @brief Set coordinates only if new values are smaller (from array).
	 * @param t Array of 3 values
	 */
	template<typename _W> inline void SetIfSmaller( _W t[3] ) {
		static_assert(std::is_arithmetic<_W>::value, "Data type must be numeric");
		x = (t[0]<x) ? t[0] : x;
		y = (t[1]<y) ? t[1] : y;
		z = (t[2]<z) ? t[2] : z;
	}

	/**
	 * @brief Multiply coordinates by corresponding values from another CTriple.
	 * @param t Another CTriple object
	 */
	template<typename _W> inline void scaleByVector(const CTriple<_W>& t)
	{
		static_assert(std::is_arithmetic<_W>::value, "CTriple<> type must be numeric");
		x *= static_cast<_T>(t.x);
		y *= static_cast<_T>(t.y);
		z *= static_cast<_T>(t.z);
	}

	// Accessors

	/**
	 * @brief Returns pointer to the array of coordinates.
	 * @return Pointer to v[0]
	 */
	inline _T * toVector() { return &v[0]; }
	/**
	 * @brief Returns const pointer to the array of coordinates.
	 * @return Pointer to v[0]
	 */
	inline const _T* toVector() const { return &v[0]; }

	/**
	 * @brief Getter for x coordinate.
	 * @return Value of x
	 */
	inline _T X() const { return x; }
	/**
	 * @brief Setter for x coordinate (chainable).
	 * @param ax New value for x
	 * @return Reference to this object
	 */
	inline CTriple<_T>& X( const _T ax ) { x = ax; return *this; }

	/**
	 * @brief Getter for y coordinate.
	 * @return Value of y
	 */
	inline _T Y() const { return y; }
	/**
	 * @brief Setter for y coordinate (chainable).
	 * @param ay New value for y
	 * @return Reference to this object
	 */
	inline CTriple<_T>& Y( const _T ay ) { y = ay; return *this; }

	/**
	 * @brief Getter for z coordinate.
	 * @return Value of z
	 */
	inline _T Z() const { return z; }
	/**
	 * @brief Setter for z coordinate (chainable).
	 * @param az New value for z
	 * @return Reference to this object
	 */
	inline CTriple<_T>& Z( const _T az ) { z = az; return *this; }

	/**
	 * @brief Getter for x coordinate cast to another type.
	 * @tparam _W Target type
	 * @return x cast to _W
	 */
	template<typename _W> _W X() const { return static_cast<_W>(x); }
	/**
	 * @brief Getter for y coordinate cast to another type.
	 * @tparam _W Target type
	 * @return y cast to _W
	 */
	template<typename _W> _W Y() const { return static_cast<_W>(y); }
	/**
	 * @brief Getter for z coordinate cast to another type.
	 * @tparam _W Target type
	 * @return z cast to _W
	 */
	template<typename _W> _W Z() const { return static_cast<_W>(z); }

	// OPERATORS

	/**
	 * @brief Access coordinate by index.
	 * @param idx Index (0, 1, 2)
	 * @return Reference to the selected coordinate
	 */
	_T& operator[](const unsigned int idx) {
		assert((idx < 3) && "Index out of range");
		return v[idx];
	}

	/**
	 * @brief Access coordinate by index (const).
	 * @param idx Index (0, 1, 2)
	 * @return Const reference to the selected coordinate
	 */
	const _T& operator[](const unsigned int idx) const {
		assert((idx < 3) && "Index out of range");
		return v[idx];
	}

	// Arithmetic operators

	/**
	 * @brief Assignment operator.
	 * @param c Another CTriple object
	 * @return Reference to this object
	 */
	CTriple<_T>& operator=(const CTriple<_T>& c) {
		if (this != &c) {
			x = c.x; y = c.y; z = c.z;
		}
		return *this;
	}

	/**
	 * @brief Negation operator (opposite vector).
	 * @return New object with negated values
	 */
	CTriple<_T> operator-() const {
		return CTriple<_T>(-x, -y, -z);
	}

	/**
	 * @brief Addition operator.
	 * @param c Second operand
	 * @return Sum of vectors
	 */
	CTriple<_T> operator+( const CTriple<_T>& c ) const {
		return CTriple<_T>( x + c.x, y + c.y, z + c.z );
	}

	/**
	 * @brief Subtraction operator.
	 * @param c Operand to subtract
	 * @return Difference of vectors
	 */
	CTriple<_T> operator-( const CTriple<_T>& c ) const {
		return CTriple<_T>( x - c.x, y - c.y, z - c.z );
	}

	/**
	 * @brief Division by scalar.
	 * @param c Scalar value
	 * @return Result of division
	 */
	CTriple<_T> operator/( const _T c ) const {
		assert(c != static_cast<_T>(0) && "Division by zero");
		return CTriple<_T>( x/c, y/c, z/c ); 
	}

	/**
	 * @brief Multiplication by scalar.
	 * @param c Scalar value
	 * @return Result of multiplication
	 */
	CTriple<_T> operator*(const _T c) const {
		return CTriple<_T>(x * c, y * c, z * c);
	}

	/**
	 * @brief Addition and assignment operator.
	 * @param c Second operand
	 * @return Reference to this object
	 */
	CTriple<_T>& operator+=( const CTriple<_T>& c ) {
		x += c.x; y += c.y; z += c.z;
		return *this;
	}

	/**
	 * @brief Subtraction and assignment operator.
	 * @param c Operand to subtract
	 * @return Reference to this object
	 */
	CTriple<_T>& operator-=( const CTriple<_T>& c ) {
		x -= c.x; y -= c.y; z -= c.z;
		return *this;
	}

	/**
	 * @brief Division by scalar and assignment operator.
	 * @param c Scalar value
	 * @return Reference to this object
	 */
	CTriple<_T>& operator/=( const _T c ) {
		assert(c != static_cast<_T>(0) && "Division by zero");
		x /= c; y /= c; z /= c;
		return *this;
	}

	/**
	 * @brief Multiplication by scalar and assignment operator.
	 * @param c Scalar value
	 * @return Reference to this object
	 */
	CTriple<_T>& operator*=(const _T c) {
		x *= c; y *= c; z *= c;
		return *this;
	}	

	// Logical operators

	/**
	 * @brief Equality operator.
	 * @param c Another object
	 * @return true if all coordinates are equal
	 */
	bool operator==( const CTriple<_T>& c ) const {
		return ( x == c.x ) && ( y == c.y ) && ( z == c.z );
	}

	/**
	 * @brief Inequality operator.
	 * @param c Another object
	 * @return true if any coordinate is different
	 */
	bool operator!=( const CTriple<_T>& c ) const {
		return ( x != c.x ) || ( y != c.y ) || ( z != c.z );
	}

	/**
	 * @brief Lexicographical less-than operator.
	 * @param c Another object
	 * @return true if this object is lexicographically less
	 */
	bool operator<( const CTriple<_T> &c ) const {
		if ( x < c.x ) return true;
		else if ( x > c.x ) return false;
		else if ( y < c.y ) return true;
		else if ( y > c.y ) return false;
		return ( z < c.z );
	}

	/**
	 * @brief Lexicographical less-than-or-equal operator.
	 * @param c Another object
	 * @return true if this object is lexicographically less or equal
	 */
	bool operator<=( const CTriple<_T> &c ) const {
		if ( x < c.x ) return true;
		else if ( x > c.x ) return false;
		else if ( y < c.y ) return true;
		else if ( y > c.y ) return false;
		return ( z <= c.z );
	}

	/**
	 * @brief Lexicographical greater-than operator.
	 * @param c Another object
	 * @return true if this object is lexicographically greater
	 */
	bool operator>( const CTriple<_T> &c ) const {
		if ( x > c.x ) return true;
		else if ( x < c.x ) return false;
		else if ( y > c.y ) return true;
		else if ( y < c.y ) return false;
		return ( z > c.z );
	}

	/**
	 * @brief Lexicographical greater-than-or-equal operator.
	 * @param c Another object
	 * @return true if this object is lexicographically greater or equal
	 */
	bool operator>=( const CTriple<_T> &c ) const {
		if ( x > c.x ) return true;
		else if ( x < c.x ) return false;
		else if ( y > c.y ) return true;
		else if ( y < c.y ) return false;
		return ( z >= c.z );
	}

};

#endif /* _TRIO_H_BY_DP_INCLUDED_ */