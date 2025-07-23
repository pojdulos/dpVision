#pragma once
#ifndef _POINT3_H_BY_DP_INCLUDED_
#define _POINT3_H_BY_DP_INCLUDED_

#include "dll_global.h"

#include <Eigen/Geometry>
#include "Triple.h"
#include "Vector3.h"

/**
 * @brief Template class representing a 3D point with arithmetic and geometric operations.
 * 
 * Inherits from CTriple and provides point-specific operations such as distance calculation,
 * box inclusion test, and transformation by matrices.
 * 
 * @tparam _T Numeric type (e.g., float, double, int)
 */
template<
	typename _T,
	typename = typename std::enable_if<std::is_arithmetic<_T>::value, _T>::type >
class DPVISION_EXPORT CPoint3 : public CTriple<_T>
{
public:
	/// Default constructor. Initializes all coordinates to 0.
	CPoint3() noexcept : CTriple<_T>() {}

	/// Constructor from three values.
	CPoint3(_T fx, _T fy, _T fz) noexcept : CTriple<_T>(fx, fy, fz) {}

	/// Constructor with all coordinates set to the same value.
	CPoint3(_T f) noexcept : CTriple<_T>(f, f, f) {}

	/// Constructor from another CTriple (possibly different type).
	template<typename _W> CPoint3(const CTriple<_W>& t) noexcept : CTriple<_T>(t) {}

	// Constructor from Eigen::Vector3
	template<typename _W> CPoint3(const Eigen::Matrix<_W, 3, 1>& t) noexcept : CTriple<_T>(t.x(), t.y(), t.z()) {}

	// Constructor from Eigen::RowVector3
	template<typename _W> CPoint3(const Eigen::Matrix<_W, 1, 3>& t) noexcept : CTriple<_T>(t.x(), t.y(), t.z()) {}

	// Constructor from Eigen::Vector4
	template<typename _W> CPoint3(const Eigen::Matrix<_W, 4, 1>& t) noexcept : CTriple<_T>(t.x(), t.y(), t.z()) {}

	// Constructor from Eigen::RowVector4
	template<typename _W> CPoint3(const Eigen::Matrix<_W, 1, 4>& t) noexcept : CTriple<_T>(t.x(), t.y(), t.z()) {}

	/**
	 * @brief Checks if the point is inside the axis-aligned box defined by min and max.
	 * @param min Minimum corner of the box
	 * @param max Maximum corner of the box
	 * @return true if the point is inside the box
	 */
	template<typename _W> bool isInBox(const CPoint3<_W>& min, const CPoint3<_W>& max) const noexcept {
		return (this->x >= min.X()) && (this->x <= max.X()) &&
		       (this->y >= min.Y()) && (this->y <= max.Y()) &&
		       (this->z >= min.Z()) && (this->z <= max.Z());
	}

	/**
	 * @brief Negation operator (returns the point with all coordinates negated).
	 * @return Negated point
	 */
	CPoint3<_T> operator-() const noexcept {
		return CPoint3<_T>(-this->x, -this->y, -this->z);
	}

	/**
	 * @brief Addition with a vector or triple.
	 * @param c Vector or triple to add
	 * @return Resulting point
	 */
	template<typename _W> CPoint3<_T> operator+(const CTriple<_W>& c) const noexcept {
		return CPoint3<_T>(this->x + c.x, this->y + c.y, this->z + c.z);
	}

	/**
	 * @brief Subtraction of two points returns a vector.
	 * @param c Point to subtract
	 * @return Vector from c to this point
	 */
	template<typename _W> CVector3<_T> operator-(const CPoint3<_W>& c) const noexcept {
		return CVector3<_T>(this->x - c.x, this->y - c.y, this->z - c.z);
	}

	/**
	 * @brief Subtraction of a vector from a point returns a point.
	 * @param c Vector to subtract
	 * @return Resulting point
	 */
	template<typename _W> CPoint3<_T> operator-(const CVector3<_W>& c) const noexcept {
		return CPoint3<_T>(this->x - c.x, this->y - c.y, this->z - c.z);
	}

	/**
	 * @brief Division by scalar.
	 * @param c Scalar value
	 * @return Resulting point
	 */
	CPoint3<_T> operator/(const _T c) const {
		assert(c != static_cast<_T>(0) && "Division by zero");
		return CPoint3<_T>(this->x / c, this->y / c, this->z / c);
	}

	/**
	 * @brief Multiplication by scalar.
	 * @param c Scalar value
	 * @return Resulting point
	 */
	CPoint3<_T> operator*(const _T c) const noexcept {
		return CPoint3<_T>(this->x * c, this->y * c, this->z * c);
	}

	/**
	 * @brief Addition and assignment with a vector or triple.
	 * @param c Vector or triple to add
	 * @return Reference to this object
	 */
	template<typename _W> CPoint3<_T>& operator+=(const CTriple<_W>& c) noexcept {
		this->x += c.x; this->y += c.y; this->z += c.z;
		return *this;
	}

	/**
	 * @brief Subtraction and assignment with a vector or triple.
	 * @param c Vector or triple to subtract
	 * @return Reference to this object
	 */
	template<typename _W> CPoint3<_T>& operator-=(const CTriple<_W>& c) noexcept {
		this->x -= c.x; this->y -= c.y; this->z -= c.z;
		return *this;
	}

	/**
	 * @brief Division by scalar and assignment.
	 * @param c Scalar value
	 * @return Reference to this object
	 */
	CPoint3<_T>& operator/=(const _T c) {
		assert(c != static_cast<_T>(0) && "Division by zero");
		this->x /= c; this->y /= c; this->z /= c;
		return *this;
	}

	/**
	 * @brief Multiplication by scalar and assignment.
	 * @param c Scalar value
	 * @return Reference to this object
	 */
	CPoint3<_T>& operator*=(const _T c) noexcept {
		this->x *= c; this->y *= c; this->z *= c;
		return *this;
	}

	/**
	 * @brief Squared distance to another point.
	 * @param p Another point
	 * @return Squared distance
	 */
	template<typename _W> double squaredDistanceTo(const CPoint3<_W>& p) const noexcept {
		double dx = p.x - this->x;
		double dy = p.y - this->y;
		double dz = p.z - this->z;
		return dx * dx + dy * dy + dz * dz;
	}

	/**
	 * @brief Euclidean distance to another point.
	 * @param p Another point
	 * @return Distance
	 */
	template<typename _W> double distanceTo(const CPoint3<_W>& p) const noexcept {
		double dx = p.x - this->x;
		double dy = p.y - this->y;
		double dz = p.z - this->z;
		return sqrt(dx * dx + dy * dy + dz * dz);
	}

	/**
	 * @brief Convert to Eigen::Vector4 (last component is 1).
	 * @return Eigen 4x1 vector
	 */
	Eigen::Matrix<_T, 4, 1> toVector4() const noexcept
	{
		Eigen::Matrix<_T, 4, 1> ev;
		ev(0, 0) = this->x;
		ev(1, 0) = this->y;
		ev(2, 0) = this->z;
		ev(3, 0) = 1;
		return ev;
	}

	/**
	 * @brief Convert to Eigen::RowVector4 (last component is 1).
	 * @return Eigen 1x4 row vector
	 */
	Eigen::Matrix<_T, 1, 4> toRowVector4() const noexcept
	{
		Eigen::Matrix<_T, 1, 4> erv;
		erv(0, 0) = this->x;		erv(0, 1) = this->y;		erv(0, 2) = this->z;		erv(0, 3) = 1;
		return erv;
	}

	/**
	 * @brief Transform by 4x4 matrix (float). The result 'w' component is handled for perspective division.
	 * @param matrix 4x4 matrix in column-major order (OpenGL style)
	 * @return Transformed point
	 */
	CPoint3<_T> transformByMatrixF(const float matrix[16]) const noexcept
	{
		CPoint3<_T> result;

		result.x = this->x * matrix[0] + this->y * matrix[4] + this->z * matrix[8] + matrix[12];
		result.y = this->x * matrix[1] + this->y * matrix[5] + this->z * matrix[9] + matrix[13];
		result.z = this->x * matrix[2] + this->y * matrix[6] + this->z * matrix[10] + matrix[14];

		_T w = this->x * matrix[3] + this->y * matrix[7] + this->z * matrix[11] + matrix[15];

		if (w != 1 && w != 0) {
			result.x /= w;
			result.y /= w;
			result.z /= w;
		}

		return result;
	}

	/**
	 * @brief Transform by 4x4 matrix (double). The result 'w' component is handled for perspective division.
	 * @param matrix 4x4 matrix in column-major order (OpenGL style)
	 * @return Transformed point
	 */
	CPoint3<_T> transformByMatrixD(const double matrix[16]) const noexcept
	{
		CPoint3<_T> result;

		result.x = this->x * matrix[0] + this->y * matrix[4] + this->z * matrix[8] + matrix[12];
		result.y = this->x * matrix[1] + this->y * matrix[5] + this->z * matrix[9] + matrix[13];
		result.z = this->x * matrix[2] + this->y * matrix[6] + this->z * matrix[10] + matrix[14];

		_T w = this->x * matrix[3] + this->y * matrix[7] + this->z * matrix[11] + matrix[15];

		if (w != 1 && w != 0) {
			result.x /= w;
			result.y /= w;
			result.z /= w;
		}

		return result;
	}

	/**
	 * @brief Transform by Eigen 4x4 matrix. The result 'w' component is handled for perspective division.
	 * @param matrix Eigen 4x4 matrix
	 * @return Transformed point
	 */
	CPoint3<_T> transformByMatrix(const Eigen::Matrix4d& matrix) const noexcept
	{
		CPoint3<_T> result;

		result.x = this->x * matrix(0,0) + this->y * matrix(0,1) + this->z * matrix(0,2) + matrix(0,3);
		result.y = this->x * matrix(1,0) + this->y * matrix(1,1) + this->z * matrix(1,2) + matrix(1,3);
		result.z = this->x * matrix(2,0) + this->y * matrix(2,1) + this->z * matrix(2,2) + matrix(2,3);

		_T w = this->x * matrix(3,0) + this->y * matrix(3,1) + this->z * matrix(3,2) + matrix(3,3);

		if (w != 1 && w != 0) {
			result.x /= w;
			result.y /= w;
			result.z /= w;
		}

		return result;
	}

	/**
	 * @brief Transform by Eigen 3x3 matrix (affine transform).
	 * @param matrix Eigen 3x3 matrix
	 * @return Transformed point
	 */
	CPoint3<_T> transformByMatrix(const Eigen::Matrix3d& matrix) const noexcept
	{
		CPoint3<_T> result;

		result.x = this->x * matrix(0, 0) + this->y * matrix(0, 1) + this->z * matrix(0, 2);
		result.y = this->x * matrix(1, 0) + this->y * matrix(1, 1) + this->z * matrix(1, 2);
		result.z = this->x * matrix(2, 0) + this->y * matrix(2, 1) + this->z * matrix(2, 2);

		return result;
	}
};

typedef CPoint3<short> CPoint3s;
typedef CPoint3<int> CPoint3i;
typedef CPoint3<float> CPoint3f;
typedef CPoint3<double> CPoint3d;

#endif /* _POINT3_H_BY_DP_INCLUDED_ */
