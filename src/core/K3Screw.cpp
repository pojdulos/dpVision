#include "K3Screw.h"
#include <iostream>

std::string K3Screw::dispMatrixXd(Eigen::MatrixXd m)
{
    std::string result;
    if (m.rows()>1) result = "[\n";
    for (int r = 0; r < m.rows(); r++) {
        result += "[";
        for (int c = 0; c < m.cols(); c++) {
            result += std::to_string(m(r, c)) + " ";
        }
        result += "]\n";
    }
    if (m.rows() > 1) result += "]\n";

    return result;
}


std::tuple<Vector3d, Vector3d> K3Screw::K3Fletch(const Vector3d& V) {
    int imin = 0;
    double minAbs = std::abs(V(0));
    for (int i = 1; i < V.size(); ++i) {
        if (std::abs(V(i)) < minAbs) {
            minAbs = std::abs(V(i));
            imin = i;
        }
    }

    Vector3d XV = Vector3d::Zero();
    XV(imin) = 1.0;

    Vector3d YV = XV.cross(V);
    YV.normalize();

    XV = YV.cross(V);
    XV = -XV.normalized();

    return std::make_tuple(XV, YV);
}

Vector3d K3Screw::K3Normalize(const Vector3d& V) {
    return V.normalized();
}

Vector3d K3Screw::K3AxisVersor(const Matrix3d& R) {
    Matrix3d A = R - Matrix3d::Identity();

    Eigen::JacobiSVD<Matrix3d> svd(A, Eigen::ComputeFullU | Eigen::ComputeFullV);

    //std::cout << "\n\nLooking for axis versor\n";
    //std::cout << "\nU1 =" << dispMatrixXd(svd.matrixU()) << std::endl;
    //std::cout << "\ns1 =" << dispMatrixXd(svd.singularValues()) << std::endl;
    //std::cout << "\nV1 =" << dispMatrixXd(svd.matrixV()) << std::endl;

    Vector3d V = svd.matrixV().col(2);

    if (V.norm() == 0)
        return Vector3d::Zero();
    else
        return V.normalized();
}

Eigen::MatrixXd K3Screw::null(const Eigen::MatrixXd& A, double rcond) {
    //std::cout << "\nA =" << dispMatrixXd(A) << std::endl;

    Eigen::JacobiSVD<Eigen::MatrixXd> svd(A, Eigen::ComputeFullU | Eigen::ComputeFullV);

    //std::cout << "\n\nLooking for null space\n";
    //std::cout << "\nU2 =" << dispMatrixXd(svd.matrixU()) << std::endl;
    //std::cout << "\ns2 =" << dispMatrixXd(svd.singularValues()) << std::endl;
    //std::cout << "\nV2 =" << dispMatrixXd(svd.matrixV()) << std::endl;

    Eigen::MatrixXd null_space;
    if (rcond < 0.0) {
        rcond = svd.singularValues().maxCoeff() * std::numeric_limits<double>::epsilon();
    }

    Eigen::MatrixXd null_mask = (svd.singularValues().array() <= rcond * svd.singularValues()(0)).cast<double>().matrix();

    null_space = svd.matrixV().rightCols(A.cols() - svd.rank()); // Pobranie kolumn null space
    null_space.transposeInPlace(); // Transpozycja, aby uzyskaæ wektory jako wiersze

    return null_space;
}

std::tuple<Vector3d, double, Vector3d, Vector3d> K3Screw::K3RigidToScrew(const Matrix4d& M) {
    Matrix3d R = M.topLeftCorner<3, 3>();
    Vector3d T = M.topRightCorner<3, 1>();

    Vector3d V = K3AxisVersor(R);
    //std::cout << "\n\nAxis Versor =" << dispMatrixXd(V) << std::endl;

    if (V.norm() == 0) {
        //std::cout << "no rotation detected" << std::endl;
        return std::make_tuple(Vector3d(1., 0., 0.), 0.0, Vector3d::Zero(), Vector3d::Zero());
    }
    else {
        Vector3d XV, YV;
        std::tie(XV, YV) = K3Fletch(V);
        Vector3d XVP = R * XV;

        double alpha = std::atan2(XVP.dot(YV), XVP.dot(XV));

        double dist = V.transpose().dot(T);
        //std::cout << "\n\ndist = " << dist << std::endl;
        Vector3d t = V * dist;

        //Vector3d D = T - t;

        //Eigen::Matrix4d temp = Eigen::MatrixXd::Zero(4, 3);
        Eigen::Matrix4d temp = Eigen::Matrix4d::Zero();
        temp.col(3).head(3) = t;
        
        Eigen::Matrix4d MP = M - temp - Eigen::Matrix4d::Identity();

        Eigen::MatrixXd D7 = null(MP);

        //std::cout << "\n\nD7 (null space result) =" << dispMatrixXd(D7) << std::endl;


        if (D7.rows() == 1)
        {
            Eigen::Vector3d D_trimmed = D7.row(0).head(3) * (1.0 / D7.row(0).tail(1)(0));
            return std::make_tuple(V, alpha, D_trimmed, t);
        }
        else
        {
            Eigen::Vector4d D(0., 0., 0., 0.);
            for (int i = 0; i < D7.rows(); i++)
            {
                if (abs(D7(i, 3)) > 0.0001)
                {
                    Eigen::Vector4d D7a = D7.row(i).transpose() * (1.0 / D7(i, 3));
                    D += D7a;
                }
            }

            Eigen::Vector3d D_trimmed = D.head(3) * (1.0 / D(3));
            return std::make_tuple(V, alpha, D_trimmed, t);
        }
    }
}

Vector3d K3Screw::K3Projection(const Vector3d& P, const Vector3d& Q, const Vector3d& v) {
    Vector3d u = { P[0] - Q[0], P[1] - Q[1], P[2] - Q[2] };
    double dot_product = u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
    double v_length_squared = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
    double scalar = dot_product / v_length_squared;
    Vector3d projection_point = { Q[0] + scalar * v[0], Q[1] + scalar * v[1], Q[2] + scalar * v[2] };
    return projection_point;
}
