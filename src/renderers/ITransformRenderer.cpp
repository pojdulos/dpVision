#include "ITransformRenderer.h"
#include "Transform.h"
#include "K3Screw.h"
#include "dpLog.h"
#include <qopengl.h>

void ITransformRenderer::render(const CTransform* _obj)
{
    CTransform* obj = (CTransform*)_obj;

    if (obj->isScrewVisible()) renderScrew(obj);
    //float matrix[16];

    //toGLMatrixF(matrix);

    //glMultMatrixf(matrix);

    glMultMatrixf(obj->toQMatrix4x4().data());
}

void ITransformRenderer::renderScrew(const CTransform* _obj, float r, float g, float b)
{
    CTransform* obj = (CTransform*)_obj;

    auto k3 = K3Screw::K3RigidToScrew(obj->toEigenMatrix4d());
    Vector3d V = std::get<0>(k3);
    double alpha = std::get<1>(k3);
    Vector3d D = std::get<2>(k3);
    Vector3d t = std::get<3>(k3);

    //std::cout << "\nV =" << dispMatrixXd(V);
    //std::cout << "\nalpha =" << rad2deg(alpha);
    //std::cout << "\nD = " << dispMatrixXd(D);
    //std::cout << "\nt = " << dispMatrixXd(t) << endl;
    dpInfo() << "alpha: " << rad2deg(alpha) << "°";
    Vector3d P = K3Screw::K3Projection({ 0., 0., 0. }, D, V);

    //std::cout << "\nnz =" << dispMatrixXd(P);

    std::vector<Vector3d> A = { P - 50 * V, P, P + 50 * V };
    
    Eigen::Quaterniond q = obj->rotation().toEigen();
    //std::cout << "\nq = [ w: " << q.w() << ", x: " << q.x() << ", y: " << q.y() << ", z: " << q.z() << "]" << std::endl;

    glPushMatrix();
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glColor3f(r, g, b);

    glLineWidth(3.0);
    glBegin(GL_LINES);
    glVertex3f(A[0][0], A[0][1], A[0][2]);
    glVertex3f(A[2][0], A[2][1], A[2][2]);
    glEnd();
    glLineWidth(1.0);
    glBegin(GL_LINES);
    glVertex3f(0., 0., 0.);
    glVertex3f(A[1][0], A[1][1], A[1][2]);
    glEnd();

    glPopAttrib();
    glPopMatrix();
}

