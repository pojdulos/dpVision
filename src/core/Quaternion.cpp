#include "Quaternion.h"

/**
 * @file Quaternion.cpp
 * @brief Implementation of the CQuaternion class for 3D rotations using quaternions.
 */

/**
 * @brief Converts the quaternion to a 4x4 OpenGL-style float matrix (column-major).
 * @param matrix Output array of 16 floats.
 */
void CQuaternion::toGLMatrixF(float* matrix)
{
    normalize();

    Eigen::Matrix3d m3 = quat_.toRotationMatrix();

    // OpenGL expects column-major order, so transpose:
    matrix[0] = (float)m3(0, 0);    matrix[4] = (float)m3(0, 1);    matrix[8] = (float)m3(0, 2);    matrix[12] = 0.0f;
    matrix[1] = (float)m3(1, 0);    matrix[5] = (float)m3(1, 1);    matrix[9] = (float)m3(1, 2);    matrix[13] = 0.0f;
    matrix[2] = (float)m3(2, 0);    matrix[6] = (float)m3(2, 1);    matrix[10] = (float)m3(2, 2);   matrix[14] = 0.0f;
    matrix[3] = 0.0f;               matrix[7] = 0.0f;               matrix[11] = 0.0f;              matrix[15] = 1.0f;
}

/**
 * @brief Converts the quaternion to a 4x4 OpenGL-style double matrix (column-major).
 * @param matrix Output array of 16 doubles.
 */
void CQuaternion::toGLMatrixD(double* matrix)
{
    normalize();

    Eigen::Matrix3d m3 = quat_.toRotationMatrix();

    // OpenGL expects column-major order, so transpose:
    matrix[0] = m3(0, 0);    matrix[4] = m3(0, 1);    matrix[8] = m3(0, 2);    matrix[12] = 0.0;
    matrix[1] = m3(1, 0);    matrix[5] = m3(1, 1);    matrix[9] = m3(1, 2);    matrix[13] = 0.0;
    matrix[2] = m3(2, 0);    matrix[6] = m3(2, 1);    matrix[10] = m3(2, 2);   matrix[14] = 0.0;
    matrix[3] = 0.0;         matrix[7] = 0.0;         matrix[11] = 0.0;        matrix[15] = 1.0;
}

/**
 * @brief Converts the quaternion to a 4x4 Eigen double matrix (homogeneous transformation).
 * @return 4x4 Eigen matrix.
 */
Eigen::Matrix4d CQuaternion::toEigenMatrix4d()
{
    normalize();

    Eigen::Matrix3d m3 = quat_.toRotationMatrix();

    Eigen::Matrix4d m;

    m(0, 0) = m3(0, 0);    m(0, 1) = m3(0, 1);    m(0, 2) = m3(0, 2);    m(0, 3) = 0.0;
    m(1, 0) = m3(1, 0);    m(1, 1) = m3(1, 1);    m(1, 2) = m3(1, 2);    m(1, 3) = 0.0;
    m(2, 0) = m3(2, 0);    m(2, 1) = m3(2, 1);    m(2, 2) = m3(2, 2);    m(2, 3) = 0.0;
    m(3, 0) = 0.0;         m(3, 1) = 0.0;         m(3, 2) = 0.0;         m(3, 3) = 1.0;

    return m;
}

/**
 * @brief Converts the quaternion to a 4x4 Qt QMatrix4x4 (homogeneous transformation).
 * @return 4x4 Qt matrix.
 */
QMatrix4x4 CQuaternion::toQMatrix4x4()
{
    normalize();

    Eigen::Matrix3d m3 = quat_.toRotationMatrix();

    QMatrix4x4 m;

    m(0, 0) = m3(0, 0);    m(0, 1) = m3(0, 1);    m(0, 2) = m3(0, 2);    m(0, 3) = 0.0;
    m(1, 0) = m3(1, 0);    m(1, 1) = m3(1, 1);    m(1, 2) = m3(1, 2);    m(1, 3) = 0.0;
    m(2, 0) = m3(2, 0);    m(2, 1) = m3(2, 1);    m(2, 2) = m3(2, 2);    m(2, 3) = 0.0;
    m(3, 0) = 0.0;         m(3, 1) = 0.0;         m(3, 2) = 0.0;         m(3, 3) = 1.0;

    return m;
}

/**
 * @brief Sets the quaternion from a 3x3 rotation matrix.
 * @param m 3x3 matrix as double**.
 */
void CQuaternion::fromRotationMatrix(double** m)
{
    Eigen::Matrix3d m3;
    m3(0, 0) = m[0][0];     m3(0, 1) = m[0][1];     m3(0, 2) = m[0][2];
    m3(1, 0) = m[1][0];     m3(1, 1) = m[1][1];     m3(1, 2) = m[1][2];
    m3(2, 0) = m[2][0];     m3(2, 1) = m[2][1];     m3(2, 2) = m[2][2];

    quat_ = Eigen::Quaterniond(m3);
}

/**
 * @brief Returns the Euler angles (XYZ order, radians) corresponding to this quaternion.
 * @return Vector of angles (x, y, z) in radians.
 */
CVector3d CQuaternion::eulerAngles()
{
    return CVector3d(quat_.toRotationMatrix().eulerAngles(0, 1, 2));
}

/**
 * @brief Returns the Euler angles (XYZ order, degrees) corresponding to this quaternion.
 * @return Vector of angles (x, y, z) in degrees.
 */
CVector3d CQuaternion::eulerAnglesDeg()
{
    Eigen::Vector3d tmp = quat_.toRotationMatrix().eulerAngles(0, 1, 2);
    Eigen::Vector3d tmp2(rad2deg(tmp[0]), rad2deg(tmp[1]), rad2deg(tmp[2]));
    return tmp2;
}

/**
 * @brief Sets the quaternion from Euler angles (XYZ order, radians).
 * @param v Vector of angles (x, y, z) in radians.
 */
void CQuaternion::fromEulerAngles(CVector3d v)
{
    *this = Eigen::AngleAxisd(v.x, Eigen::Vector3d::UnitX())
        * Eigen::AngleAxisd(v.y, Eigen::Vector3d::UnitY())
        * Eigen::AngleAxisd(v.z, Eigen::Vector3d::UnitZ());
}

/**
 * @brief Sets the quaternion from Euler angles (XYZ order, radians).
 * @param x Angle around X axis (radians).
 * @param y Angle around Y axis (radians).
 * @param z Angle around Z axis (radians).
 */
void CQuaternion::fromEulerAngles(double x, double y, double z)
{
    *this = Eigen::AngleAxisd(x, Eigen::Vector3d::UnitX())
        * Eigen::AngleAxisd(y, Eigen::Vector3d::UnitY())
        * Eigen::AngleAxisd(z, Eigen::Vector3d::UnitZ());
}

/**
 * @brief Sets the quaternion from Euler angles (XYZ order, degrees).
 * @param v Vector of angles (x, y, z) in degrees.
 */
void CQuaternion::fromEulerAnglesDeg(CVector3d v)
{
    *this = Eigen::AngleAxisd(deg2rad(v.x), Eigen::Vector3d::UnitX())
        * Eigen::AngleAxisd(deg2rad(v.y), Eigen::Vector3d::UnitY())
        * Eigen::AngleAxisd(deg2rad(v.z), Eigen::Vector3d::UnitZ());
}

/**
 * @brief Sets the quaternion from Euler angles (XYZ order, degrees).
 * @param x Angle around X axis (degrees).
 * @param y Angle around Y axis (degrees).
 * @param z Angle around Z axis (degrees).
 */
void CQuaternion::fromEulerAnglesDeg(double x, double y, double z)
{
    *this = Eigen::AngleAxisd(deg2rad(x), Eigen::Vector3d::UnitX())
        * Eigen::AngleAxisd(deg2rad(y), Eigen::Vector3d::UnitY())
        * Eigen::AngleAxisd(deg2rad(z), Eigen::Vector3d::UnitZ());
}

/**
 * @brief Quaternion logarithm.
 * log([cos(a), n*sin(a)]) = [0, a*n]
 * @return Logarithm of the quaternion.
 */
CQuaternion CQuaternion::log()
{
    double t = acos(w());
    double module = sqrt(x() * x() + y() * y() + z() * z());
    if (module < EPSILON)
        t = 0.0;
    else
        t /= module;

    return CQuaternion(0.0, x() * t, y() * t, z() * t);
}

/**
 * @brief Quaternion exponential.
 * exp([0, a*n]) = [cos(a), n*sin(a)]
 * @return Exponential of the quaternion.
 */
CQuaternion CQuaternion::exp()
{
    double om = sqrt(x() * x() + y() * y() + z() * z());
    double sinom;
    if (fabs(om) < EPSILON)
        sinom = 1.0;
    else
        sinom = sin(om) / om;

    return CQuaternion(cos(om), sinom * x(), sinom * y(), sinom * z());
}

/**
 * @brief Quaternion exponentiation (fractional rotation).
 * @param exponent Fraction of the rotation.
 * @return Quaternion raised to the given exponent.
 */
CQuaternion CQuaternion::pow(double exponent) {
    // Straight from the definition (no optimization, but works)
    CQuaternion qlog = log();
    double w = qlog.w();
    double x = qlog.x() * exponent;
    double y = qlog.y() * exponent;
    double z = qlog.z() * exponent;
    CQuaternion scaled(w, x, y, z);
    return scaled.exp();
}
