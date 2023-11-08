#pragma once
//#ifndef QUATERNION_H
//#define QUATERNION_H

#include <cmath>
#include <iostream>

#include "Utilities.h"
#include<Eigen/Geometry>
#include <QtGui/QMatrix4x4>
#include "Point3.h"
#include "Vector3.h"

#define EPSILON 0.0001

// Basic class representing quaternions

// NOTE : within the quaternion class I assume that magnitude of each quaternion equals 'one'.
// With this assumption in mind calculations on quaternions can be implemented more efficently.

#include "Wektor3D.h"

//using namespace Eigen;

class  DPVISION_EXPORT CQuaternion : public Eigen::Quaterniond
{
public:
	using Eigen::Quaterniond::operator*;

	CQuaternion() : Eigen::Quaterniond() { setIdentity(); }
	CQuaternion(double angle, const CVector3d& axis) : Eigen::Quaterniond(Eigen::AngleAxisd(angle, Eigen::Vector3d(axis.x, axis.y, axis.z))) {}
	CQuaternion(double w, double x, double y, double z) : Eigen::Quaterniond(w, x, y, z) {}

	template<typename _Q> CQuaternion(const Eigen::AngleAxis<_Q>& aa) : Eigen::Quaterniond(aa) {}
	template<typename _Q> CQuaternion(const Eigen::MatrixBase<_Q>& mm) : Eigen::Quaterniond(mm) {}
	template<typename _Q> CQuaternion(const Eigen::Quaternion<_Q>& quat) : Eigen::Quaterniond(quat) {}

	CQuaternion(const QQuaternion qq) : Eigen::Quaterniond(qq.scalar(), qq.x(), qq.y(), qq.z()) {}

	~CQuaternion() {}

	inline QQuaternion toQt() { return QQuaternion(w(), x(), y(), z()); }

	CVector3d eulerAngles();
	CVector3d eulerAnglesDeg();
	void fromEulerAngles(CVector3d v);
	void fromEulerAngles(double x, double y, double z);
	void fromEulerAnglesDeg(CVector3d v);
	void fromEulerAnglesDeg(double x, double y, double z);

	void fromRotationMatrix(double** m);

	void setIdentity() { *this = Identity(); }
	
	inline CQuaternion negate() { return Eigen::Quaterniond(-w(), -x(), -y(), -z()); }
	
	inline void multAndSet(const CQuaternion& quat) { (*this) *= quat; }

	// Quaternion dot product ('cosine of the angle' between two quaternions)
	inline double dotProduct(const CQuaternion& quat) { return this->dot(quat);	}

	// Quaternion difference (angular displacement from one orientation to another)
	CQuaternion	diff(const CQuaternion& quat) { return (this->inverse() * quat); }

	// Quaternion logarithm ( log ([cos(a) n sin(a)]) = [ 0 a*n ] )
	CQuaternion log();

	// Quaternion exponential function ( exp ([0 a*n]) = [ cos(a)  n*sin(a) ] )
	CQuaternion exp();

	// Quaternion exponentiation (takes a fraction of the rotation)
	CQuaternion pow(double exponent);

	// Creates an OpenGL-style rotation matrix from the quaternion
	void toGLMatrixF(float* matrix);
	void toGLMatrixD(double* matrix);

	// Creates an inverse of OpenGL-style rotation matrix from the quaternion
	inline void toInvertedGLMatrixF(float* matrix) { ((CQuaternion)inverse()).toGLMatrixF(matrix); }
	inline void toInvertedGLMatrixD(double* matrix) { ((CQuaternion)inverse()).toGLMatrixD(matrix); }

	Eigen::Matrix4d toEigenMatrix4d();
	QMatrix4x4 toQMatrix4x4();

	template<typename _W> CVector3<_W> operator*(CVector3<_W> p) const {
		return CVector3<_W>(*this * p.toVector3());
	};

	template<typename _W> CPoint3<_W> operator*(CPoint3<_W> p) const {
		return CPoint3<_W>(*this * p.toVector3());
	};

	//// Returns the rotation angle
	//double GetAngle() const {
	//	return 2.0 * acos(w());
	//}

	//// Extracts the axis of rotation
	//void GetVector(double* vector) const {
	//	double temp = 1.0 - w() * w();		// (sin x)^2 = 1.0 - (cos x)^2

	//	if (temp <= 0.0005)		// Identity quaternion or numerical imprecision.
	//	{
	//		vector[0] = 0.0;
	//		vector[1] = 0.0;
	//		vector[2] = 0.0;
	//	}
	//	else
	//	{
	//		temp = 1.0 / sqrt(temp);
	//		vector[0] = x() * temp;
	//		vector[1] = y() * temp;
	//		vector[2] = z() * temp;
	//	}
	//}
};


//#endif
