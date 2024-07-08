#include "Quaternion.h"

// Creates an OpenGL-style rotation matrix from the quaternion
//inline void CQuaternion::toGLMatrixF(float* matrix) {
//	normalize();
//
//	double xx = x() * x();
//	double yy = y() * y();
//	double zz = z() * z();
//	
//	// First row
//	matrix[0] = float(1.0 - 2.0 * (yy + zz));
//	matrix[1] = float(2.0 * (x() * y() + z() * w()));
//	matrix[2] = float(2.0 * (x() * z() - y() * w()));
//	matrix[3] = float(0.0);
//
//	// Second row
//	matrix[4] = float(2.0 * (x() * y() - z() * w()));
//	matrix[5] = float(1.0 - 2.0 * (xx + zz));
//	matrix[6] = float(2.0 * (z() * y() + x() * w()));
//	matrix[7] = float(0.0);
//
//	// Third row
//	matrix[8] = float(2.0 * (x() * z() + y() * w()));
//	matrix[9] = float(2.0 * (y() * z() - x() * w()));
//	matrix[10] = float(1.0 - 2.0 * (xx + yy));
//	matrix[11] = float(0.0);
//
//	// Fourth row
//	matrix[12] = 0.0f;
//	matrix[13] = 0.0f;
//	matrix[14] = 0.0f;
//	matrix[15] = 1.0f;
//
//	// Now matrix[] is a 4x4 homogeneous matrix that can be applied to an OpenGL Matrix
//}

inline void CQuaternion::toGLMatrixF(float* matrix)
{
	normalize();

	Eigen::Matrix3d m3 = toRotationMatrix();

	//OpenGLMatrix is column-first, so transpose:
	matrix[0] = (float)m3(0, 0);		matrix[4] = (float)m3(0, 1);		matrix[8] = (float)m3(0, 2);		matrix[12] = 0.0f;
	matrix[1] = (float)m3(1, 0);		matrix[5] = (float)m3(1, 1);		matrix[9] = (float)m3(1, 2);		matrix[13] = 0.0f;
	matrix[2] = (float)m3(2, 0);		matrix[6] = (float)m3(2, 1);		matrix[10] = (float)m3(2, 2);		matrix[14] = 0.0f;
	matrix[3] = 0.0f;					matrix[7] = 0.0f;					matrix[11] = 0.0f;					matrix[15] = 1.0f;
}


//inline void CQuaternion::toGLMatrixD(double* matrix) {
//	normalize();
//
//	double xx = x() * x();
//	double yy = y() * y();
//	double zz = z() * z();
//
//	// First row
//	matrix[0] = 1.0 - 2.0 * (yy + zz);
//	matrix[1] = 2.0 * (x() * y() + z() * w());
//	matrix[2] = 2.0 * (x() * z() - y() * w());
//	matrix[3] = 0.0;
//
//	// Second row
//	matrix[4] = 2.0 * (x() * y() - z() * w());
//	matrix[5] = 1.0 - 2.0 * (xx + zz);
//	matrix[6] = 2.0 * (z() * y() + x() * w());
//	matrix[7] = 0.0;
//
//	// Third row
//	matrix[8] = 2.0 * (x() * z() + y() * w());
//	matrix[9] = 2.0 * (y() * z() - x() * w());
//	matrix[10] = 1.0 - 2.0 * (xx + yy);
//	matrix[11] = 0.0;
//
//	// Fourth row
//	matrix[12] = 0.0;
//	matrix[13] = 0.0;
//	matrix[14] = 0.0;
//	matrix[15] = 1.0;
//
//	// Now matrix[] is a 4x4 homogeneous matrix that can be applied to an OpenGL Matrix
//}


inline void CQuaternion::toGLMatrixD(double* matrix)
{
	normalize();

	Eigen::Matrix3d m3 = toRotationMatrix();

	//OpenGLMatrix is column-first, so transpose:
	matrix[0] = m3(0, 0);		matrix[4] = m3(0, 1);		matrix[ 8] = m3(0, 2);		matrix[12] = 0.0;
	matrix[1] = m3(1, 0);		matrix[5] = m3(1, 1);		matrix[ 9] = m3(1, 2);		matrix[13] = 0.0;
	matrix[2] = m3(2, 0);		matrix[6] = m3(2, 1);		matrix[10] = m3(2, 2);		matrix[14] = 0.0;
	matrix[3] = 0.0;			matrix[7] = 0.0;			matrix[11] = 0.0;			matrix[15] = 1.0;
}

Eigen::Matrix4d CQuaternion::toEigenMatrix4d()
{
	normalize();

	Eigen::Matrix3d m3 = toRotationMatrix();

	Eigen::Matrix4d m;

	m(0, 0) = m3(0, 0);		m(0, 1) = m3(0, 1);		m(0, 2) = m3(0, 2);		m(0, 3) = 0.0;
	m(1, 0) = m3(1, 0);		m(1, 1) = m3(1, 1);		m(1, 2) = m3(1, 2);		m(1, 3) = 0.0;
	m(2, 0) = m3(2, 0);		m(2, 1) = m3(2, 1);		m(2, 2) = m3(2, 2);		m(2, 3) = 0.0;
	m(3, 0) = 0.0;			m(3, 1) = 0.0;			m(3, 2) = 0.0;			m(3, 3) = 1.0;

	return m;
}

QMatrix4x4 CQuaternion::toQMatrix4x4()
{
	normalize();

	Eigen::Matrix3d m3 = toRotationMatrix();
	
	QMatrix4x4 m;

	m(0, 0) = m3(0, 0);		m(0, 1) = m3(0, 1);		m(0, 2) = m3(0, 2);		m(0, 3) = 0.0;
	m(1, 0) = m3(1, 0);		m(1, 1) = m3(1, 1);		m(1, 2) = m3(1, 2);		m(1, 3) = 0.0;
	m(2, 0) = m3(2, 0);		m(2, 1) = m3(2, 1);		m(2, 2) = m3(2, 2);		m(2, 3) = 0.0;
	m(3, 0) = 0.0;			m(3, 1) = 0.0;			m(3, 2) = 0.0;			m(3, 3) = 1.0;

	return m;
}



void CQuaternion::fromRotationMatrix(double** m)
{
	Eigen::Matrix3d m3;
	m3(0, 0) = m[0][0];		m3(0, 1) = m[0][1];		m3(0, 2) = m[0][2];
	m3(1, 0) = m[1][0];		m3(1, 1) = m[1][1];		m3(1, 2) = m[1][2];
	m3(2, 0) = m[2][0];		m3(2, 1) = m[2][1];		m3(2, 2) = m[2][2];

	*this = m3;
}


CVector3d CQuaternion::eulerAngles()
{
	return CVector3d(toRotationMatrix().eulerAngles(0, 1, 2));
}

CVector3d CQuaternion::eulerAnglesDeg()
{
	Eigen::Vector3d tmp = toRotationMatrix().eulerAngles(0, 1, 2);
	Eigen::Vector3d tmp2(rad2deg(tmp[0]), rad2deg(tmp[1]), rad2deg(tmp[2]));
	//std::cout << "EULER: " << tmp2 << std::endl;
	return tmp2;
	//return rad2deg(CVector3d(toRotationMatrix().eulerAngles(0, 1, 2)));
}

void CQuaternion::fromEulerAngles(CVector3d v)
{
	*this = Eigen::AngleAxisd(v.x, Eigen::Vector3d::UnitX())
		* Eigen::AngleAxisd(v.y, Eigen::Vector3d::UnitY())
		* Eigen::AngleAxisd(v.z, Eigen::Vector3d::UnitZ());
}

void CQuaternion::fromEulerAngles(double x, double y, double z)
{
	*this = Eigen::AngleAxisd(x, Eigen::Vector3d::UnitX())
		* Eigen::AngleAxisd(y, Eigen::Vector3d::UnitY())
		* Eigen::AngleAxisd(z, Eigen::Vector3d::UnitZ());
}

void CQuaternion::fromEulerAnglesDeg(CVector3d v)
{
	*this = Eigen::AngleAxisd(deg2rad(v.x), Eigen::Vector3d::UnitX())
		* Eigen::AngleAxisd(deg2rad(v.y), Eigen::Vector3d::UnitY())
		* Eigen::AngleAxisd(deg2rad(v.z), Eigen::Vector3d::UnitZ());
}

void CQuaternion::fromEulerAnglesDeg(double x, double y, double z)
{
	*this = Eigen::AngleAxisd(deg2rad(x), Eigen::Vector3d::UnitX())
		* Eigen::AngleAxisd(deg2rad(y), Eigen::Vector3d::UnitY())
		* Eigen::AngleAxisd(deg2rad(z), Eigen::Vector3d::UnitZ());
}

// Quaternion logarithm ( log ([cos(a) n sin(a)]) = [ 0 a*n ] )

inline CQuaternion CQuaternion::log()
{
	double		t = acos(w());
	double		module;				// Module of the axis vector
	module = sqrt(x() * x() + y() * y() + z() * z());
	if (module < EPSILON)
		t = 0.0;
	else
		t /= module;

	return CQuaternion(0.0, x() * t, y() * t, z() * t);
}

// Quaternion exponential function ( exp ([0 a*n]) = [ cos(a)  n*sin(a) ] )

inline CQuaternion CQuaternion::exp()
{
	CQuaternion QResult;
	double om, sinom;

	om = sqrt(x() * x() + y() * y() + z() * z());
	if (fabs(om) < EPSILON)
		sinom = 1.0;
	else
		sinom = sin(om) / om;

	return CQuaternion(cos(om), sinom * x(), sinom * y(), sinom * z());
}

// Quaternion exponentiation (takes a fraction of the rotation)

inline CQuaternion CQuaternion::pow(double exponent) {
	/*	if (fabs(w) > 0.999f)	// Exponentiation of the identity quaternion results in identity quaternion
	return *this;

	// Extract the half angle alpha
	float alpha = acosf(w);

	// Computer new alpha value
	float newAlpha = alpha * exponent;

	// Compute new value
	CQuaternion QResult;

	QResult.w = cosf(newAlpha);
	float mult = sinf(newAlpha)/sinf(alpha);
	QResult.x = x * mult;
	QResult.y = y * mult;
	QResult.z = z * mult;			*/

	// straight from the definition (no optimization really but works ...)

	CQuaternion QResult = log();
	QResult.x() = QResult.x() * exponent;
	QResult.y() = QResult.y() * exponent;
	QResult.z() = QResult.z() * exponent;
	QResult = QResult.exp();

	return QResult;
}

