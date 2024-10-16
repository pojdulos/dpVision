#pragma once
#ifndef _VECTOR3_H_BY_DP_INCLUDED_
#define _VECTOR3_H_BY_DP_INCLUDED_

#include "dll_global.h"

#include <vector>
#include <math.h>
#include "Triple.h"
#include <Eigen/Geometry>

template<
	typename _T,
	typename = typename std::enable_if<std::is_arithmetic<_T>::value, _T>::type >  class DPVISION_EXPORT CVector3 : public CTriple<_T>
{
public:
	CVector3() : CTriple<_T>() {};

	template<typename _W> CVector3(_W fx, _W fy, _W fz) : CTriple<_T>(fx, fy, fz) {};

	CVector3(_T f) : CTriple<_T>(f, f, f) {};

	//template<typename _W> CVector3(const CTriple<_W>& t) : CTriple<_T>(t) {};

	template<typename _W> CVector3(const CVector3<_W> &t) : CTriple<_T>(t) {};

	// constructor from Eigen::Vector3
	template<typename _W> CVector3(const Eigen::Matrix<_W, 3, 1>& t) : CTriple<_T>(t.x(), t.y(), t.z()) {};

	// constructor from Eigen::RowVector3
	template<typename _W> CVector3(const Eigen::Matrix<_W, 1, 3>& t) : CTriple<_T>(t.x(), t.y(), t.z()) {};

	// constructor from Eigen::Vector4
	template<typename _W> CVector3(const Eigen::Matrix<_W, 4, 1>& t) : CTriple<_T>(t.x(), t.y(), t.z()) {};

	// constructor from Eigen::RowVector4
	template<typename _W> CVector3(const Eigen::Matrix<_W, 1, 4>& t) : CTriple<_T>(t.x(), t.y(), t.z()) {};


	template<typename _U, typename _W> CVector3(const CTriple<_U> &p, const CTriple<_W> &k) {
		static_assert(std::is_arithmetic<_U>::value, "CVector3 member type must be numeric");
		static_assert(std::is_arithmetic<_W>::value, "CVector3 member type must be numeric");
		this->x = static_cast<_T>(k.x) - static_cast<_T>(p.x);
		this->y = static_cast<_T>(k.y) - static_cast<_T>(p.y);
		this->z = static_cast<_T>(k.z) - static_cast<_T>(p.z);
	};

	// modulo ka�dego elementu przez skalar c, z zachowaniem znaku:
	// (-c) < x <= c, (-c) < y <= c, (-c) < z <= c
	template<typename _W> CVector3 modulo( const _W c ) {
		while (this->x > c) this->x -= c;
		while (this->y > c) this->y -= c;
		while (this->z > c) this->z -= c;
		while (this->x <= -c) this->x += c;
		while (this->y <= -c) this->y += c;
		while (this->z <= -c) this->z += c;
		return CVector3(this->x, this->y, this->z);
	};


	CVector3<_T> operator-() const {
		return CVector3<_T>(-this->x, -this->y, -this->z);
	};


	CVector3<_T> operator+(const CTriple<_T>& c) const {
		return CVector3<_T>(this->x + c.x, this->y + c.y, this->z + c.z);
	};

	CVector3<_T> operator+=(const CTriple<_T>& c) {
		return CVector3<_T>(this->x += c.x, this->y += c.y, this->z += c.z);
	};

	CVector3<_T> operator-(const CTriple<_T>& c) const {
		return CVector3<_T>(this->x - c.x, this->y - c.y, this->z - c.z);
	};

	CVector3<_T> operator-=(const CTriple<_T>& c) {
		return CVector3<_T>(this->x -= c.x, this->y -= c.y, this->z -= c.z);
	};

	CVector3<_T> operator/(const _T c) const {
		return CVector3<_T>(this->x / c, this->y / c, this->z / c);
	};

	// iloczyn ze skalarem
	CVector3<_T> operator*(const _T c) const {
		return CVector3<_T>(this->x * c, this->y * c, this->z * c);
	};

	CVector3<_T> operator/=(const _T c) {
		return CVector3<_T>(this->x /= c, this->y /= c, this->z /= c);
	};

	// iloczyn ze skalarem
	CVector3<_T> operator*=(const _T c) {
		return CVector3<_T>(this->x *= c, this->y *= c, this->z *= c);
	};


	// iloczyn wektorowy
	template<typename _W> CVector3 crossProduct(const CVector3<_W> &c) const {
		return CVector3<_T>(this->y * c.Z() - this->z * c.Y(), this->z * c.X() - this->x * c.Z(), this->x * c.Y() - this->y * c.X());
	};

	// iloczyn wektorowy ( wywo�uje metod� v.crossProduct(w) )
	template<typename _W> CVector3 operator*(const CVector3<_W> &c) const {	return crossProduct( c ); };


	// iloczyn skalarny 
	template<typename _W> double dotProduct(const CVector3<_W> &c) const {
		return (double)this->x * c.X() + (double)this->y * c.Y() + (double)this->z * c.Z();
	};

	// normalizuje wektor
	void normalize() {
		double d = sqrt((double)this->x*this->x + (double)this->y*this->y + (double)this->z*this->z);
		if (d) {
			this->x = static_cast<_T>((double)this->x / d);
			this->y = static_cast<_T>((double)this->y / d);
			this->z = static_cast<_T>((double)this->z / d);
		}
	};

	// zwraca wektor znormalizowany, ale nie modyfikuje orygina�u
	CVector3 getNormalized() {
		double d = sqrt((double)this->x*this->x + (double)this->y*this->y + (double)this->z*this->z);
		if (d) {
			return CVector3<_T>(static_cast<_T>((double)this->x / d), static_cast<_T>((double)this->y / d), static_cast<_T>((double)this->z / d));
		}
		return CVector3<_T>(0.0, 0.0, 0.0);
	};

	// negacja
	inline void invert() { this->x = -this->x;	this->y = -this->y;	this->z = -this->z; };

	// d�ugo�� wektora
	double length() { return sqrt((double)this->x*this->x + (double)this->y*this->y + (double)this->z*this->z); };

	// kwadrat d�ugo�ci wektora
	double squaredLength() { return (double)this->x*this->x + (double)this->y*this->y + (double)this->z*this->z; };


	template<typename _W> double angleTo(const CVector3<_W> &c) {
		return acos(dotProduct(c));
	}

	// export to Eigen::Vector4<Type>
	Eigen::Matrix<_T, 4, 1> toVector4()
	{
		Eigen::Matrix<_T, 4, 1> ev;
		ev(0, 0) = this->x;
		ev(1, 0) = this->y;
		ev(2, 0) = this->z;
		ev(3, 0) = 0;
		return ev;
	}

	// export to Eigen::RowVector4<Type>
	Eigen::Matrix<_T, 1, 4> toRowVector4()
	{
		Eigen::Matrix<_T, 1, 4> erv;
		erv(0, 0) = this->x;		erv(0, 1) = this->y;		erv(0, 2) = this->z;		erv(0, 3) = 0;
		return erv;
	}



	// Multiply by matrix 4x4 (result 'w' component is discarded)
	CVector3<_T> transformByMatrixF(float matrix[16])
	{
		CVector3<_T> result;

		result.x = this->x * matrix[0] + this->y * matrix[4] + this->z * matrix[8];
		result.y = this->x * matrix[1] + this->y * matrix[5] + this->z * matrix[9];
		result.z = this->x * matrix[2] + this->y * matrix[6] + this->z * matrix[10];

		return result;
	}

	CVector3<_T> transformByMatrixD(double matrix[16])
	{
		CVector3<_T> result;

		result.x = this->x * matrix[0] + this->y * matrix[4] + this->z * matrix[8];
		result.y = this->x * matrix[1] + this->y * matrix[5] + this->z * matrix[9];
		result.z = this->x * matrix[2] + this->y * matrix[6] + this->z * matrix[10];

		return result;
	}

	CVector3<_T> transformByMatrix(Eigen::Matrix3d matrix)
	{
		CVector3<_T> result;

		result.x = this->x * matrix(0, 0) + this->y * matrix(0, 1) + this->z * matrix(0, 2);
		result.y = this->x * matrix(1, 0) + this->y * matrix(1, 1) + this->z * matrix(1, 2);
		result.z = this->x * matrix(2, 0) + this->y * matrix(2, 1) + this->z * matrix(2, 2);

		return result;
	}


	CVector3<_T> transform(const _T matrix[3][3]) const {
		return {
			matrix[0][0] * this->x + matrix[0][1] * this->y + matrix[0][2] * this->z,
			matrix[1][0] * this->x + matrix[1][1] * this->y + matrix[1][2] * this->z,
			matrix[2][0] * this->x + matrix[2][1] * this->y + matrix[2][2] * this->z
		};
	}

	static inline CVector3<_T> XAxis() { return CVector3<_T>(1, 0, 0); }
	static inline CVector3<_T> YAxis() { return CVector3<_T>(0, 1, 0); }
	static inline CVector3<_T> ZAxis() { return CVector3<_T>(0, 0, 1); }
};

typedef CVector3<short> CVector3s;
typedef CVector3<int> CVector3i;
typedef CVector3<float> CVector3f;
typedef CVector3<double> CVector3d;

#define CWektor3D CVector3f

//class DPVISION_EXPORT CArray_OfNormals : public std::vector<CVector3f> {};

#endif /* _VECTOR3_H_BY_DP_INCLUDED_ */