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

/**
 * @brief Class representing a 3D rotation using quaternions.
 *
 * This class wraps Eigen::Quaterniond and provides a convenient interface for 3D rotations,
 * including conversion to/from Euler angles, rotation matrices, and integration with Qt and OpenGL.
 * All quaternions are assumed to be normalized (unit quaternions).
 */
class DPVISION_EXPORT CQuaternion
{
private:
    /// Internal Eigen quaternion representation.
    Eigen::Quaterniond quat_;

public:
    /**
     * @brief Default constructor. Initializes to identity quaternion.
     */
    CQuaternion() : quat_(Eigen::Quaterniond::Identity()) {}

    /**
     * @brief Construct quaternion from rotation angle and axis.
     * @param angle Rotation angle in radians.
     * @param axis 3D axis of rotation (should be normalized).
     */
    CQuaternion(double angle, const CVector3d& axis)
        : quat_(Eigen::AngleAxisd(angle, Eigen::Vector3d(axis.x, axis.y, axis.z))) {}

    /**
     * @brief Construct quaternion from components.
     * @param w Scalar part.
     * @param x X component of vector part.
     * @param y Y component of vector part.
     * @param z Z component of vector part.
     */
    CQuaternion(double w, double x, double y, double z)
        : quat_(w, x, y, z) {}

    /**
     * @brief Construct quaternion from Eigen::AngleAxis.
     */
    template<typename _Q> CQuaternion(const Eigen::AngleAxis<_Q>& aa) : quat_(aa) {}

    /**
     * @brief Construct quaternion from Eigen matrix (3x3 or 4x4).
     */
    template<typename _Q> CQuaternion(const Eigen::MatrixBase<_Q>& mm) : quat_(mm) {}

    /**
     * @brief Construct quaternion from Eigen quaternion.
     */
    template<typename _Q> CQuaternion(const Eigen::Quaternion<_Q>& quat) : quat_(quat) {}

    /**
     * @brief Construct quaternion from Qt's QQuaternion.
     */
    CQuaternion(const QQuaternion& qq) : quat_(qq.scalar(), qq.x(), qq.y(), qq.z()) {}

    ~CQuaternion() {}

    // --- Getters and Setters ---

    /**
     * @brief Get scalar (w) component.
     */
    double w() const { return quat_.w(); }
    /**
     * @brief Get x component.
     */
    double x() const { return quat_.x(); }
    /**
     * @brief Get y component.
     */
    double y() const { return quat_.y(); }
    /**
     * @brief Get z component.
     */
    double z() const { return quat_.z(); }

    /**
     * @brief Set scalar (w) component.
     */
    void setW(double w) { quat_.w() = w; }
    /**
     * @brief Set x component.
     */
    void setX(double x) { quat_.x() = x; }
    /**
     * @brief Set y component.
     */
    void setY(double y) { quat_.y() = y; }
    /**
     * @brief Set z component.
     */
    void setZ(double z) { quat_.z() = z; }
    /**
     * @brief Set all components.
     */
    void set(double w, double x, double y, double z) { quat_ = Eigen::Quaterniond(w, x, y, z); }

    /**
     * @brief Get the underlying Eigen quaternion (read-only).
     */
    const Eigen::Quaterniond& toEigen() const { return quat_; }

    /**
     * @brief Convert to Qt's QQuaternion.
     */
    inline QQuaternion toQt() const { return QQuaternion(w(), x(), y(), z()); }

    // --- Operators ---

    /**
     * @brief Quaternion multiplication (combines rotations).
     */
    CQuaternion operator*(const CQuaternion& other) const { return CQuaternion(quat_ * other.quat_); }
    /**
     * @brief In-place quaternion multiplication.
     */
    CQuaternion& operator*=(const CQuaternion& other) { quat_ *= other.quat_; return *this; }
    /**
     * @brief Inverse quaternion (opposite rotation).
     */
    CQuaternion inverse() const { return CQuaternion(quat_.inverse()); }
    /**
     * @brief Normalize the quaternion to unit length.
     */
    void normalize() { quat_.normalize(); }

    // --- Conversion and Utility Methods ---

    /**
     * @brief Get Euler angles (XYZ order, in radians).
     * @return Vector of angles (x, y, z).
     */
    CVector3d eulerAngles();
    /**
     * @brief Get Euler angles (XYZ order, in degrees).
     * @return Vector of angles (x, y, z) in degrees.
     */
    CVector3d eulerAnglesDeg();
    /**
     * @brief Set quaternion from Euler angles (XYZ order, radians).
     */
    void fromEulerAngles(CVector3d v);
    /**
     * @brief Set quaternion from Euler angles (XYZ order, radians).
     */
    void fromEulerAngles(double x, double y, double z);
    /**
     * @brief Set quaternion from Euler angles (XYZ order, degrees).
     */
    void fromEulerAnglesDeg(CVector3d v);
    /**
     * @brief Set quaternion from Euler angles (XYZ order, degrees).
     */
    void fromEulerAnglesDeg(double x, double y, double z);

    /**
     * @brief Set quaternion from 3x3 rotation matrix.
     * @param m 3x3 matrix as double**.
     */
    void fromRotationMatrix(double** m);

    /**
     * @brief Set to identity quaternion (no rotation).
     */
    void setIdentity() { quat_ = Eigen::Quaterniond::Identity(); }

    /**
     * @brief Negate the quaternion (reverse all components).
     */
    inline CQuaternion negate() const { return CQuaternion(-w(), -x(), -y(), -z()); }

    /**
     * @brief Multiply by another quaternion and set the result.
     */
    inline void multAndSet(const CQuaternion& quat) { quat_ *= quat.quat_; }

    /**
     * @brief Dot product (cosine of the angle between two quaternions).
     */
    inline double dotProduct(const CQuaternion& quat) const { return quat_.dot(quat.quat_); }

    /**
     * @brief Quaternion difference (angular displacement from this to another).
     */
    CQuaternion diff(const CQuaternion& quat) const { return this->inverse() * quat; }

    /**
     * @brief Quaternion logarithm.
     * log([cos(a), n*sin(a)]) = [0, a*n]
     */
    CQuaternion log();

    /**
     * @brief Quaternion exponential.
     * exp([0, a*n]) = [cos(a), n*sin(a)]
     */
    CQuaternion exp();

    /**
     * @brief Quaternion exponentiation (fractional rotation).
     * @param exponent Fraction of the rotation.
     */
    CQuaternion pow(double exponent);

    // --- Matrix Conversion ---

    /**
     * @brief Convert to OpenGL-style 4x4 float matrix (column-major).
     * @param matrix Output array of 16 floats.
     */
    void toGLMatrixF(float* matrix);

    /**
     * @brief Convert to OpenGL-style 4x4 double matrix (column-major).
     * @param matrix Output array of 16 doubles.
     */
    void toGLMatrixD(double* matrix);

    /**
     * @brief Convert to inverted OpenGL-style 4x4 float matrix.
     * @param matrix Output array of 16 floats.
     */
    inline void toInvertedGLMatrixF(float* matrix) const { this->inverse().toGLMatrixF(matrix); }

    /**
     * @brief Convert to inverted OpenGL-style 4x4 double matrix.
     * @param matrix Output array of 16 doubles.
     */
    inline void toInvertedGLMatrixD(double* matrix) const { this->inverse().toGLMatrixD(matrix); }

    /**
     * @brief Convert to Eigen 4x4 double matrix.
     * @return 4x4 Eigen matrix.
     */
    Eigen::Matrix4d toEigenMatrix4d();

    /**
     * @brief Convert to Qt's QMatrix4x4.
     * @return 4x4 Qt matrix.
     */
    QMatrix4x4 toQMatrix4x4();

    // --- Vector/Point Transformation ---

    /**
     * @brief Rotate a 3D vector by this quaternion.
     */
    template<typename _W> CVector3<_W> operator*(CVector3<_W> p) const {
        return CVector3<_W>(quat_ * p.toVector3());
    }

    /**
     * @brief Rotate a 3D point by this quaternion.
     */
    template<typename _W> CPoint3<_W> operator*(CPoint3<_W> p) const {
        return CPoint3<_W>(quat_ * p.toVector3());
    }
};
