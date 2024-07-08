#pragma once
#ifndef _POINT3_H_BY_DP_INCLUDED_
#define _POINT3_H_BY_DP_INCLUDED_

#include "dll_global.h"

#include <Eigen/Geometry>
#include "Triple.h"
#include "Vector3.h"

template<
	typename _T,
	typename = typename std::enable_if<std::is_arithmetic<_T>::value, _T>::type >  class DPVISION_EXPORT CPoint3 : public CTriple<_T>
{
public:
	CPoint3() : CTriple<_T>() {};
	CPoint3(_T fx, _T fy, _T fz) :CTriple<_T>(fx, fy, fz) {};
	CPoint3(_T f) : CTriple<_T>(f, f, f) {};


	template<typename _W> CPoint3(const CTriple<_W>& t) :CTriple<_T>(t) {};
	//template<typename _W> CPoint3(const CPoint3<_W> &t) :CTriple<_T>(t) {};

	// constructor from Eigen::Vector3
	template<typename _W> CPoint3(const Eigen::Matrix<_W, 3, 1>& t) : CTriple<_T>(t.x(), t.y(), t.z()) {};

	// constructor from Eigen::RowVector3
	template<typename _W> CPoint3(const Eigen::Matrix<_W, 1, 3>& t) : CTriple<_T>(t.x(), t.y(), t.z()) {};

	// constructor from Eigen::Vector4
	template<typename _W> CPoint3(const Eigen::Matrix<_W, 4, 1>& t) : CTriple<_T>(t.x(), t.y(), t.z()) {};

	// constructor from Eigen::RowVector4
	template<typename _W> CPoint3(const Eigen::Matrix<_W, 1, 4>& t) : CTriple<_T>(t.x(), t.y(), t.z()) {};




	template<typename _W> bool isInBox(CPoint3<_W> min, CPoint3<_W> max) { return (this->x >= min.X()) && (this->x <= max.X()) && (this->y >= min.Y()) && (this->y <= max.Y()) && (this->z >= min.Z()) && (this->z <= max.Z()); }

	CPoint3<_T> operator-() const {
		return CPoint3<_T>(-this->x, -this->y, -this->z);
	};

	template<typename _W> CPoint3<_T> operator+(const CTriple<_W>& c) const {
		return CPoint3<_T>(this->x + c.x, this->y + c.y, this->z + c.z);
	};

	template<typename _W> CPoint3<_T> operator+=(const CTriple<_W>& c) {
		return CPoint3<_T>(this->x += c.x, this->y += c.y, this->z += c.z);
	};

	// Point minus Point is Vector
	template<typename _W> CVector3<_T> operator-(const CPoint3<_W>& c) const {
		return CVector3<_T>(this->x - c.x, this->y - c.y, this->z - c.z);
	};

	// Point minus Vector is Point
	template<typename _W> CPoint3<_T> operator-(const CVector3<_W>& c) const {
		return CPoint3<_T>(this->x - c.x, this->y - c.y, this->z - c.z);
	};

	//template<typename _W> CPoint3<_T> operator-(const CTriple<_W>& c) const {
	//	return CPoint3<_T>(x - c.x, y - c.y, z - c.z);
	//};

	template<typename _W> CPoint3<_T> operator-=(const CTriple<_W>& c) {
		return CPoint3<_T>(this->x -= c.x, this->y -= c.y, this->z -= c.z);
	};

	CPoint3<_T> operator/(const _T c) const {
		return CPoint3<_T>(this->x / c, this->y / c, this->z / c);
	};

	CPoint3<_T> operator*(const _T c) const {
		return CPoint3<_T>(this->x * c, this->y * c, this->z * c);
	};

	CPoint3<_T> operator/=(const _T c) {
		return CPoint3<_T>(this->x /= c, this->y /= c, this->z /= c);
	};

	CPoint3<_T> operator*=(const _T c) {
		return CPoint3<_T>(this->x *= c, this->y *= c, this->z *= c);
	};

	template<typename _W> double squaredDistanceTo(CPoint3<_W> p) {
		double dx = p.x - this->x;
		double dy = p.y - this->y;
		double dz = p.z - this->z;

		return dx * dx + dy * dy + dz * dz;
	}

	template<typename _W> double distanceTo(CPoint3<_W> p) {
		double dx = p.x - this->x;
		double dy = p.y - this->y;
		double dz = p.z - this->z;

		return sqrt(dx * dx + dy * dy + dz * dz);
	}

	// export to Eigen::Vector4<Type>
	Eigen::Matrix<_T, 4, 1> toVector4()
	{
		Eigen::Matrix<_T, 4, 1> ev;
		ev(0, 0) = this->x;
		ev(1, 0) = this->y;
		ev(2, 0) = this->z;
		ev(3, 0) = 1;
		return ev;
	}

	// export to Eigen::RowVector4<Type>
	Eigen::Matrix<_T, 1, 4> toRowVector4()
	{
		Eigen::Matrix<_T, 1, 4> erv;
		erv(0, 0) = this->x;		erv(0, 1) = this->y;		erv(0, 2) = this->z;		erv(0, 3) = 1;
		return erv;
	}

	// Multiply by matrix 4x4 (result 'w' component is discarded)
	// OpenGL (column-first) format of the matrix
	CPoint3<_T> transformByMatrixF(float matrix[16])
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

	// OpenGL (column-first) format of the matrix
	CPoint3<_T> transformByMatrixD(double matrix[16])
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

	CPoint3<_T> transformByMatrix(Eigen::Matrix4d matrix)
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

	CPoint3<_T> transformByMatrix(Eigen::Matrix3d matrix)
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
