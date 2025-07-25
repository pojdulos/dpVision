#pragma once
#ifndef _VECTOR3_H_BY_DP_INCLUDED_
#define _VECTOR3_H_BY_DP_INCLUDED_

#include "dll_global.h"

#include <vector>
#include <math.h>
#include "Triple.h"
#include <Eigen/Geometry>

/**
 * @brief Template class representing a 3D vector with arithmetic operations and Eigen interoperability.
 * 
 * Inherits from CTriple and adds vector-specific operations such as cross product, normalization,
 * transformation, and conversion to Eigen types.
 * 
 * @tparam _T Numeric type (e.g., float, double, int)
 */
template<
	typename _T,
	typename = typename std::enable_if<std::is_arithmetic<_T>::value, _T>::type >
class DPVISION_EXPORT CVector3 : public CTriple<_T>
{
public:
	/**
	 * @brief Default constructor. Initializes all coordinates to 0.
	 */
	CVector3() noexcept : CTriple<_T>() {}

	/**
	 * @brief Constructor from three values.
	 * @param fx Value for x
	 * @param fy Value for y
	 * @param fz Value for z
	 */
	template<typename _W> CVector3(_W fx, _W fy, _W fz) noexcept : CTriple<_T>(fx, fy, fz) {}

	/**
	 * @brief Constructor with all coordinates set to the same value.
	 * @param f Value for x, y, z
	 */
	CVector3(_T f) noexcept : CTriple<_T>(f, f, f) {}

	/**
	 * @brief Copy constructor from another CVector3 of possibly different type.
	 * @param t Another CVector3 object
	 */
	template<typename _W> CVector3(const CVector3<_W> &t) noexcept : CTriple<_T>(t) {}

	/**
	 * @brief Constructor from Eigen::Vector3.
	 * @param t Eigen 3x1 vector
	 */
	template<typename _W> CVector3(const Eigen::Matrix<_W, 3, 1>& t) noexcept : CTriple<_T>(t.x(), t.y(), t.z()) {}

	/**
	 * @brief Constructor from Eigen::RowVector3.
	 * @param t Eigen 1x3 row vector
	 */
	template<typename _W> CVector3(const Eigen::Matrix<_W, 1, 3>& t) noexcept : CTriple<_T>(t.x(), t.y(), t.z()) {}

	/**
	 * @brief Constructor from Eigen::Vector4 (uses first three components).
	 * @param t Eigen 4x1 vector
	 */
	template<typename _W> CVector3(const Eigen::Matrix<_W, 4, 1>& t) noexcept : CTriple<_T>(t.x(), t.y(), t.z()) {}

	/**
	 * @brief Constructor from Eigen::RowVector4 (uses first three components).
	 * @param t Eigen 1x4 row vector
	 */
	template<typename _W> CVector3(const Eigen::Matrix<_W, 1, 4>& t) noexcept : CTriple<_T>(t.x(), t.y(), t.z()) {}

	/**
	 * @brief Construct vector as difference between two points.
	 * @param p Start point
	 * @param k End point
	 */
	template<typename _U, typename _W> CVector3(const CTriple<_U> &p, const CTriple<_W> &k) noexcept {
		static_assert(std::is_arithmetic<_U>::value, "CVector3 member type must be numeric");
		static_assert(std::is_arithmetic<_W>::value, "CVector3 member type must be numeric");
		this->x = static_cast<_T>(k.x) - static_cast<_T>(p.x);
		this->y = static_cast<_T>(k.y) - static_cast<_T>(p.y);
		this->z = static_cast<_T>(k.z) - static_cast<_T>(p.z);
	}

	/**
	 * @brief Modulo each element by scalar c, preserving sign: (-c) < x <= c, etc.
	 * @param c Scalar value
	 * @return Modulo-ed vector
	 */
	template<typename _W> CVector3 modulo(const _W c) {
		while (this->x > c) this->x -= c;
		while (this->y > c) this->y -= c;
		while (this->z > c) this->z -= c;
		while (this->x <= -c) this->x += c;
		while (this->y <= -c) this->y += c;
		while (this->z <= -c) this->z += c;
		return CVector3(this->x, this->y, this->z);
	}

	/**
	 * @brief Negation operator.
	 * @return Negated vector
	 */
	CVector3<_T> operator-() const noexcept {
		return CVector3<_T>(-this->x, -this->y, -this->z);
	}

	/**
	 * @brief Vector addition.
	 * @param c Vector to add
	 * @return Sum of vectors
	 */
	CVector3<_T> operator+(const CTriple<_T>& c) const noexcept {
		return CVector3<_T>(this->x + c.x, this->y + c.y, this->z + c.z);
	}

	/**
	 * @brief Vector subtraction.
	 * @param c Vector to subtract
	 * @return Difference of vectors
	 */
	CVector3<_T> operator-(const CTriple<_T>& c) const noexcept {
		return CVector3<_T>(this->x - c.x, this->y - c.y, this->z - c.z);
	}

	/**
	 * @brief Division by scalar.
	 * @param c Scalar value
	 * @return Result of division
	 */
	CVector3<_T> operator/(const _T c) const {
		assert(c != static_cast<_T>(0) && "Division by zero");
		return CVector3<_T>(this->x / c, this->y / c, this->z / c);
	}

	/**
	 * @brief Multiplication by scalar.
	 * @param c Scalar value
	 * @return Result of multiplication
	 */
	CVector3<_T> operator*(const _T c) const noexcept {
		return CVector3<_T>(this->x * c, this->y * c, this->z * c);
	}

	//-------------------------------------------

	/**
	 * @brief Addition and assignment operator.
	 * @param c Vector to add
	 * @return Reference to this object
	 */
	CVector3<_T>& operator+=(const CTriple<_T>& c) noexcept {
		this->x += c.x; this->y += c.y; this->z += c.z;
		return *this;
	}

	/**
	 * @brief Subtraction and assignment operator.
	 * @param c Vector to subtract
	 * @return Reference to this object
	 */
	CVector3<_T>& operator-=(const CTriple<_T>& c) noexcept {
		this->x -= c.x; this->y -= c.y; this->z -= c.z;
		return *this;
	}

	/**
	 * @brief Division by scalar and assignment operator.
	 * @param c Scalar value
	 * @return Reference to this object
	 */
	CVector3<_T>& operator/=(const _T c) {
		assert(c != static_cast<_T>(0) && "Division by zero");
		this->x /= c; this->y /= c; this->z /= c;
		return *this;
	}

	/**
	 * @brief Multiplication by scalar and assignment operator.
	 * @param c Scalar value
	 * @return Reference to this object
	 */
	CVector3<_T>& operator*=(const _T c) noexcept {
		this->x *= c; this->y *= c; this->z *= c;
		return *this;
	}

	/**
	 * @brief Cross product with another vector.
	 * @param c Another vector
	 * @return Cross product vector
	 */
	template<typename _W> CVector3 crossProduct(const CVector3<_W> &c) const noexcept {
		return CVector3<_T>(this->y * c.Z() - this->z * c.Y(), this->z * c.X() - this->x * c.Z(), this->x * c.Y() - this->y * c.X());
	}

	/**
	 * @brief Cross product operator (calls crossProduct).
	 * @param c Another vector
	 * @return Cross product vector
	 */
	template<typename _W> CVector3 operator*(const CVector3<_W> &c) const noexcept {	return crossProduct( c ); }

	/**
	 * @brief Dot product with another vector.
	 * @param c Another vector
	 * @return Dot product (scalar)
	 */
	template<typename _W> double dotProduct(const CVector3<_W> &c) const noexcept {
		return (double)this->x * c.X() + (double)this->y * c.Y() + (double)this->z * c.Z();
	}

	/**
	 * @brief Normalize the vector in place.
	 */
	void normalize() noexcept {
		double d = sqrt((double)this->x*this->x + (double)this->y*this->y + (double)this->z*this->z);
		if (d) {
			this->x = static_cast<_T>((double)this->x / d);
			this->y = static_cast<_T>((double)this->y / d);
			this->z = static_cast<_T>((double)this->z / d);
		}
	}

	/**
	 * @brief Get a normalized copy of the vector (does not modify original).
	 * @return Normalized vector
	 */
	CVector3<_T> getNormalized() const noexcept {
		double d = sqrt((double)this->x*this->x + (double)this->y*this->y + (double)this->z*this->z);
		if (d) {
			return CVector3<_T>(static_cast<_T>((double)this->x / d), static_cast<_T>((double)this->y / d), static_cast<_T>((double)this->z / d));
		}
		return CVector3<_T>(0.0, 0.0, 0.0);
	}

	/**
	 * @brief Invert the vector (negate all coordinates).
	 */
	inline void invert() noexcept { this->x = -this->x;	this->y = -this->y;	this->z = -this->z; }

	/**
	 * @brief Get the length (magnitude) of the vector.
	 * @return Vector length
	 */
	double length() const noexcept { return sqrt((double)this->x*this->x + (double)this->y*this->y + (double)this->z*this->z); }

	/**
	 * @brief Get the squared length of the vector.
	 * @return Squared vector length
	 */
	double squaredLength() const noexcept { return (double)this->x*this->x + (double)this->y*this->y + (double)this->z*this->z; }

	/**
	 * @brief Compute the angle to another vector (in radians).
	 * @param c Another vector
	 * @return Angle in radians
	 */
	template<typename _W> double angleTo(const CVector3<_W> &c) const {
		double len1 = this->length();
		double len2 = c.length();
		if (len1 == 0.0 || len2 == 0.0) return 0.0;
		double cosTheta = dotProduct(c) / (len1 * len2);
		// Clamp to [-1, 1] to avoid NaN due to floating point errors
		if (cosTheta > 1.0) cosTheta = 1.0;
		if (cosTheta < -1.0) cosTheta = -1.0;
		return acos(cosTheta);
	}

	/**
	 * @brief Convert to Eigen::Vector4 (last component is 0).
	 * @return Eigen 4x1 vector
	 */
	Eigen::Matrix<_T, 4, 1> toVector4() const noexcept
	{
		Eigen::Matrix<_T, 4, 1> ev;
		ev(0, 0) = this->x;
		ev(1, 0) = this->y;
		ev(2, 0) = this->z;
		ev(3, 0) = 0;
		return ev;
	}

	/**
	 * @brief Convert to Eigen::RowVector4 (last component is 0).
	 * @return Eigen 1x4 row vector
	 */
	Eigen::Matrix<_T, 1, 4> toRowVector4() const noexcept
	{
		Eigen::Matrix<_T, 1, 4> erv;
		erv(0, 0) = this->x;		erv(0, 1) = this->y;		erv(0, 2) = this->z;		erv(0, 3) = 0;
		return erv;
	}

	/**
	 * @brief Transform by 4x4 matrix (float). The result 'w' component is discarded.
	 * @param matrix 4x4 matrix in column-major order (OpenGL style)
	 * @return Transformed vector
	 */
	CVector3<_T> transformByMatrixF(const float matrix[16]) const noexcept
	{
		CVector3<_T> result;

		result.x = this->x * matrix[0] + this->y * matrix[4] + this->z * matrix[8];
		result.y = this->x * matrix[1] + this->y * matrix[5] + this->z * matrix[9];
		result.z = this->x * matrix[2] + this->y * matrix[6] + this->z * matrix[10];

		return result;
	}

	/**
	 * @brief Transform by 4x4 matrix (double). The result 'w' component is discarded.
	 * @param matrix 4x4 matrix in column-major order (OpenGL style)
	 * @return Transformed vector
	 */
	CVector3<_T> transformByMatrixD(const double matrix[16]) const noexcept
	{
		CVector3<_T> result;

		result.x = this->x * matrix[0] + this->y * matrix[4] + this->z * matrix[8];
		result.y = this->x * matrix[1] + this->y * matrix[5] + this->z * matrix[9];
		result.z = this->x * matrix[2] + this->y * matrix[6] + this->z * matrix[10];

		return result;
	}

	/**
	 * @brief Transform by Eigen 3x3 matrix.
	 * @param matrix Eigen 3x3 matrix
	 * @return Transformed vector
	 */
	CVector3<_T> transformByMatrix(const Eigen::Matrix3d& matrix) const noexcept
	{
		CVector3<_T> result;

		result.x = this->x * matrix(0, 0) + this->y * matrix(0, 1) + this->z * matrix(0, 2);
		result.y = this->x * matrix(1, 0) + this->y * matrix(1, 1) + this->z * matrix(1, 2);
		result.z = this->x * matrix(2, 0) + this->y * matrix(2, 1) + this->z * matrix(2, 2);

		return result;
	}

	/**
	 * @brief Transform by 3x3 C array matrix.
	 * @param matrix 3x3 array
	 * @return Transformed vector
	 */
	CVector3<_T> transform(const _T matrix[3][3]) const noexcept {
		return {
			matrix[0][0] * this->x + matrix[0][1] * this->y + matrix[0][2] * this->z,
			matrix[1][0] * this->x + matrix[1][1] * this->y + matrix[1][2] * this->z,
			matrix[2][0] * this->x + matrix[2][1] * this->y + matrix[2][2] * this->z
		};
	}

	/**
	 * @brief Get the X axis unit vector (1,0,0).
	 * @return X axis vector
	 */
	static inline CVector3<_T> XAxis() noexcept { return CVector3<_T>(1, 0, 0); }
	/**
	 * @brief Get the Y axis unit vector (0,1,0).
	 * @return Y axis vector
	 */
	static inline CVector3<_T> YAxis() noexcept { return CVector3<_T>(0, 1, 0); }
	/**
	 * @brief Get the Z axis unit vector (0,0,1).
	 * @return Z axis vector
	 */
	static inline CVector3<_T> ZAxis() noexcept { return CVector3<_T>(0, 0, 1); }
};

typedef CVector3<short> CVector3s;
typedef CVector3<int> CVector3i;
typedef CVector3<float> CVector3f;
typedef CVector3<double> CVector3d;

#define CWektor3D CVector3f

//class DPVISION_EXPORT CArray_OfNormals : public std::vector<CVector3f> {};

#endif /* _VECTOR3_H_BY_DP_INCLUDED_ */