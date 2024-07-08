#pragma once

#include "dll_global.h"

#include <iostream>
#include <Eigen/Dense>

typedef Eigen::Matrix<double, 4, 4> Matrix4d;
typedef Eigen::Matrix<double, 3, 3> Matrix3d;
typedef Eigen::Vector3d Vector3d;
typedef Eigen::Vector4d Vector4d;

DPVISION_EXPORT std::string dispMatrixXd(Eigen::MatrixXd m);
DPVISION_EXPORT std::tuple<Vector3d, Vector3d> K3Fletch(const Vector3d& V);
DPVISION_EXPORT Vector3d K3Normalize(const Vector3d& V);
DPVISION_EXPORT Vector3d K3AxisVersor(const Matrix3d& R);
DPVISION_EXPORT Eigen::MatrixXd null(const Eigen::MatrixXd& A, double rcond = -1.0);
DPVISION_EXPORT std::tuple<Vector3d, double, Vector3d, Vector3d> K3RigidToScrew(const Matrix4d& M);
DPVISION_EXPORT Vector3d K3Projection(const Vector3d& P, const Vector3d& Q, const Vector3d& v);
