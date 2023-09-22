#include "Transform.h"

#include "AP.h"

#include "MainApplication.h"

CTransform::CTransform()
{
	m_bLocked = false;
	m_bRelocateCtr = false;

	m_origin = 0.0;

	m_rot.setIdentity();
	m_tra = 0.0;
	m_sca = 1.0;
}

// Copy constructor
CTransform::CTransform(const CTransform &t)
{
	m_rot = t.m_rot;
	m_tra = t.m_tra;
	m_sca = t.m_sca;
	m_origin = t.m_origin;

	m_bLocked = t.m_bLocked;
	m_bRelocateCtr = t.m_bRelocateCtr;
}

CTransform::~CTransform()
{
}

void CTransform::reset() {
	if (!m_bLocked)
	{
		m_rot.setIdentity();
		m_tra = 0.0;
		m_sca = 1.0;
		m_origin = 0.0;
	}
}

/*
obr�t wok� zadanej osi przechodz�cej przez [0,0,0]
Wp�ywa tylko na macierz transformacji. Nie modyfikuje wsp�rz�dnych punkt�w
*/
void  CTransform::rotateAroundAxis(CVector3d axis, double angle, bool internal)
{
	if (m_bLocked) return;

	if (internal)
	{
		m_rot *= CQuaternion(angle, axis);
		m_rot.normalize();
	}
	else
	{
		CQuaternion invRot = m_rot.inverse();

		// przeliczam origin (tu translacj�) do wsp�rz�dnych wewn�trznych obiektu
		// przed wykonaniem obrotu, ale nie stosuj� skalowania
		// wiec nie trzeba go b�dzie potem skalowa� spowrotem
		Eigen::Vector3d tmpOrigin = invRot * m_tra.toVector3();

		// OBR�T wok� osi przekszta�conej do wsp�rz�dnych obiektu
		m_rot *= CQuaternion( angle, Eigen::Vector3d( invRot * axis.toVector3() ) );
		m_rot.normalize();

		// przeliczam origin (tu translacj�) do wsp�rz�dnych zewn�trzych
		m_tra = m_rot * tmpOrigin;
	}
}



/*
obr�t wok� zadanej osi przechodz�cej przez origin
Zostanie wykonana rotacja wok� osi przechodz�cej przez punkt [0,0,0] oraz translacja by �rodek obrotu pozosta� w tym samym miejscu.
Wp�ywa tylko na macierz transformacji. Nie modyfikuje wsp�rz�dnych punkt�w.
*/
void CTransform::rotateAroundAxis(CPoint3d origin, CVector3d axis, double angle, bool internal)
{
	if (m_bLocked) return;

	if (internal)
	{
		CPoint3d oldOrigin = m_rot * origin.toVector3();
		oldOrigin.scaleByVector(m_sca);

		CQuaternion invRot = m_rot.inverse();
		Eigen::Vector3d tmpOrigin = invRot * oldOrigin.toVector3();

		m_rot *= CQuaternion(angle, axis);
		m_rot.normalize();

		CPoint3d newOrigin = m_rot * tmpOrigin;
		translate(oldOrigin - newOrigin);
	}
	else
	{
		CQuaternion invRot = m_rot.inverse();

		CPoint3d oldOrigin = origin - m_tra;
		
		Eigen::Vector3d tmpOrigin = invRot * oldOrigin.toVector3();

		m_rot *= CQuaternion(angle, Eigen::Vector3d(invRot * axis.toVector3()));
		m_rot.normalize();

		CPoint3d newOrigin = m_rot * tmpOrigin;
		translate(oldOrigin - newOrigin);
	}
}




/*
z�o�enie obrot�w - zawsze wok� punktu [0,0,0]. Osie obrotu: xAxis, yAxis i zAxis 
Wp�ywa tylko na macierz transformacji. Nie modyfikuje wsp�rz�dnych punkt�w
K�ty podajemy w radianach.
*/
void CTransform::rotate(std::vector<std::pair<CVector3d, double>> axisAngle, bool internal)
{
	if (m_bLocked) return;

	if (internal)
	{
		for (auto aa : axisAngle)
		{
			m_rot *= CQuaternion(aa.second, aa.first);
		}
		m_rot.normalize();
	}
	else
	{
		CQuaternion invRot = m_rot.inverse();

		Eigen::Vector3d tmpOrigin = invRot * m_tra.toVector3();

		for (auto aa : axisAngle)
		{
			m_rot *= CQuaternion(aa.second, invRot * aa.first.toVector3());
		}
		m_rot.normalize();

		m_tra = m_rot * tmpOrigin;
	}
}


/*
z�o�enie obrot�w wok� zadanego punktu
Zostanie wykonana rotacja wok� punktu [0,0,0] oraz translacja by �rodek obrotu pozosta� w tym samym miejscu.
Wp�ywa tylko na macierz transformacji. Nie modyfikuje wsp�rz�dnych punkt�w.
K�ty podajemy w radianach.
*/
void CTransform::rotate(CPoint3d origin, std::vector<std::pair<CVector3d,double>> axisAngle, bool internal)
{
	if (m_bLocked) return;

	if (internal)
	{
		CPoint3d oldOrigin = m_rot * origin.toVector3();
		oldOrigin.scaleByVector(m_sca);

		CQuaternion invRot = m_rot.inverse();
		Eigen::Vector3d tmpOrigin = invRot * oldOrigin.toVector3();

		for (auto aa : axisAngle)
		{
			m_rot *= CQuaternion(aa.second, aa.first);
		}
		m_rot.normalize();

		CPoint3d newOrigin = m_rot * tmpOrigin;
		translate(oldOrigin - newOrigin);
	}
	else
	{
		CQuaternion invRot = m_rot.inverse();

		CPoint3d oldOrigin = origin - m_tra;

		Eigen::Vector3d tmpOrigin = invRot * oldOrigin.toVector3();

		for (auto aa : axisAngle)
		{
			m_rot *= CQuaternion(aa.second, invRot * aa.first.toVector3());
		}

		m_rot.normalize();

		CPoint3d newOrigin = m_rot * tmpOrigin;
		translate(oldOrigin - newOrigin);
	}
}

void CTransform::rotateByMatrix3x3(double m[9])
{
	CQuaternion qRot = Eigen::Matrix3d(m);

	qRot.normalize();

	m_rot *= qRot;
	m_rot.normalize();
}


const CVector3d& CTransform::translate(CVector3d ruch)
{
	if (!m_bLocked)
	{
		m_tra += ruch;
	}
	return m_tra;
};


void CTransform::fromGLMatrixF(float* matrix)
{
	m_tra.Set(matrix[12], matrix[13], matrix[14]);

	double sx = CVector3d(matrix[0], matrix[1], matrix[2]).length();
	double sy = CVector3d(matrix[4], matrix[5], matrix[6]).length();
	double sz = CVector3d(matrix[8], matrix[9], matrix[10]).length();

	m_sca = CPoint3d(sx, sy, sz);

	//double m00 = matrix[0] / sx;		double m01 = matrix[4] / sy;		double m02 = matrix[8] / sz;
	//double m10 = matrix[1] / sx;		double m11 = matrix[5] / sy;		double m12 = matrix[9] / sz;
	//double m20 = matrix[2] / sx;		double m21 = matrix[6] / sy;		double m22 = matrix[10] / sz;
	//m_rot.FromMatrix(m00, m01, m02, m10, m11, m12, m20, m21, m22);

	Eigen::Matrix3d m3;
	m3(0, 0) = (double)matrix[0] / sx;		m3(0, 1) = (double)matrix[4] / sy;		m3(0, 2) = (double)matrix[8] / sz;
	m3(1, 0) = (double)matrix[1] / sx;		m3(1, 1) = (double)matrix[5] / sy;		m3(1, 2) = (double)matrix[9] / sz;
	m3(2, 0) = (double)matrix[2] / sx;		m3(2, 1) = (double)matrix[6] / sy;		m3(2, 2) = (double)matrix[10] / sz;

	m_rot = m3;
}

void CTransform::fromGLMatrixD(double* matrix)
{
	m_tra.Set(matrix[12], matrix[13], matrix[14]);

	double sx = CVector3d(matrix[0], matrix[1], matrix[2]).length();
	double sy = CVector3d(matrix[4], matrix[5], matrix[6]).length();
	double sz = CVector3d(matrix[8], matrix[9], matrix[10]).length();

	m_sca = CPoint3d(sx, sy, sz);

	//double m00 = matrix[0] / sx;		double m01 = matrix[4] / sy;		double m02 = matrix[8] / sz;
	//double m10 = matrix[1] / sx;		double m11 = matrix[5] / sy;		double m12 = matrix[9] / sz;
	//double m20 = matrix[2] / sx;		double m21 = matrix[6] / sy;		double m22 = matrix[10] / sz;
	//m_rot.FromMatrix(m00, m01, m02, m10, m11, m12, m20, m21, m22);

	Eigen::Matrix3d m3;
	m3(0, 0) = matrix[0] / sx;		m3(0, 1) = matrix[4] / sy;		m3(0, 2) = matrix[8] / sz;
	m3(1, 0) = matrix[1] / sx;		m3(1, 1) = matrix[5] / sy;		m3(1, 2) = matrix[9] / sz;
	m3(2, 0) = matrix[2] / sx;		m3(2, 1) = matrix[6] / sy;		m3(2, 2) = matrix[10] / sz;

	m_rot = m3;
}

void CTransform::fromRowMatrixD(double* matrix)
{
	m_tra.Set(matrix[3], matrix[7], matrix[11]);

	double sx = CVector3d(matrix[0], matrix[4], matrix[8]).length();
	double sy = CVector3d(matrix[1], matrix[5], matrix[9]).length();
	double sz = CVector3d(matrix[2], matrix[6], matrix[10]).length();

	m_sca = CPoint3d(sx, sy, sz);

	//double m00 = matrix[0] / sx;		double m01 = matrix[1] / sy;		double m02 = matrix[2] / sz;
	//double m10 = matrix[4] / sx;		double m11 = matrix[5] / sy;		double m12 = matrix[6] / sz;
	//double m20 = matrix[8] / sx;		double m21 = matrix[9] / sy;		double m22 = matrix[10] / sz;
	//m_rot.FromMatrix(m00, m01, m02, m10, m11, m12, m20, m21, m22);

	Eigen::Matrix3d m3;
	m3(0, 0) = matrix[0] / sx;		m3(0, 1) = matrix[1] / sy;		m3(0, 2) = matrix[2] / sz;
	m3(1, 0) = matrix[4] / sx;		m3(1, 1) = matrix[5] / sy;		m3(1, 2) = matrix[6] / sz;
	m3(2, 0) = matrix[8] / sx;		m3(2, 1) = matrix[9] / sy;		m3(2, 2) = matrix[10] / sz;

	m_rot = m3;
}

void CTransform::fromEigenMatrix4d(Eigen::Matrix4d m)
{
	m_tra = CVector3d(m(0, 3), m(1, 3), m(2, 3));

	double sx = CVector3d(m(0, 0), m(1, 0), m(2, 0)).length();
	double sy = CVector3d(m(0, 1), m(1, 1), m(2, 1)).length();
	double sz = CVector3d(m(0, 2), m(1, 2), m(2, 2)).length();

	m_sca = CPoint3d(sx, sy, sz);

	Eigen::Matrix3d m3;
	m3(0, 0) = m(0, 0) / sx;		m3(0, 1) = m(0, 1) / sy;		m3(0, 2) = m(0, 2) / sz;
	m3(1, 0) = m(1, 0) / sx;		m3(1, 1) = m(1, 1) / sy;		m3(1, 2) = m(1, 2) / sz;
	m3(2, 0) = m(2, 0) / sx;		m3(2, 1) = m(2, 1) / sy;		m3(2, 2) = m(2, 2) / sz;

	m_rot = m3;
}

void CTransform::fromQMatrix4x4(QMatrix4x4 m)
{
	m_tra = CVector3d(m(0, 3), m(1, 3), m(2, 3));

	double sx = CVector3d(m(0, 0), m(1, 0), m(2, 0)).length();
	double sy = CVector3d(m(0, 1), m(1, 1), m(2, 1)).length();
	double sz = CVector3d(m(0, 2), m(1, 2), m(2, 2)).length();

	m_sca = CPoint3d(sx, sy, sz);

	Eigen::Matrix3d m3;
	m3(0, 0) = (double)m(0, 0) / sx;		m3(0, 1) = (double)m(0, 1) / sy;		m3(0, 2) = (double)m(0, 2) / sz;
	m3(1, 0) = (double)m(1, 0) / sx;		m3(1, 1) = (double)m(1, 1) / sy;		m3(1, 2) = (double)m(1, 2) / sz;
	m3(2, 0) = (double)m(2, 0) / sx;		m3(2, 1) = (double)m(2, 1) / sy;		m3(2, 2) = (double)m(2, 2) / sz;

	m_rot = m3;
}


void CTransform::toGLMatrixF( float *matrix )
{
	m_rot.toGLMatrixF(matrix);

	matrix[0] *= m_sca.x;
	matrix[1] *= m_sca.x;
	matrix[2] *= m_sca.x;

	matrix[4] *= m_sca.y;
	matrix[5] *= m_sca.y;
	matrix[6] *= m_sca.y;

	matrix[8] *= m_sca.z;
	matrix[9] *= m_sca.z;
	matrix[10] *= m_sca.z;

	matrix[12] = m_tra.X();
	matrix[13] = m_tra.Y();
	matrix[14] = m_tra.Z();
}

void CTransform::toGLMatrixD(double* matrix)
{
	m_rot.toGLMatrixD(matrix);

	matrix[0] *= m_sca.x;
	matrix[1] *= m_sca.x;
	matrix[2] *= m_sca.x;

	matrix[4] *= m_sca.y;
	matrix[5] *= m_sca.y;
	matrix[6] *= m_sca.y;

	matrix[8] *= m_sca.z;
	matrix[9] *= m_sca.z;
	matrix[10] *= m_sca.z;

	matrix[12] = m_tra.X();
	matrix[13] = m_tra.Y();
	matrix[14] = m_tra.Z();
}

void CTransform::toRowMatrixD(double* matrix)
{
	double tmpMatrix[16];
	m_rot.toGLMatrixD(tmpMatrix); //UWAGA: macierz rotacji jest w formacie OpenGL (kolumnowym)

	// transponuje macierz rotacji i mno�� przez skal�
	matrix[0] = tmpMatrix[0] * m_sca.x;
	matrix[1] = tmpMatrix[4] * m_sca.y;
	matrix[2] = tmpMatrix[8] * m_sca.z;

	matrix[4] = tmpMatrix[1] * m_sca.x;
	matrix[5] = tmpMatrix[5] * m_sca.y;
	matrix[6] = tmpMatrix[9] * m_sca.z;

	matrix[8] = tmpMatrix[2] * m_sca.x;
	matrix[9] = tmpMatrix[6] * m_sca.y;
	matrix[10] = tmpMatrix[10] * m_sca.z;

	matrix[3] = m_tra.X();
	matrix[7] = m_tra.Y();
	matrix[11] = m_tra.Z();

	matrix[12] = 0.0;
	matrix[13] = 0.0;
	matrix[14] = 0.0;
	matrix[15] = 1.0;
}

Eigen::Matrix4d CTransform::toEigenMatrix4d()
{
	Eigen::Matrix4d m = m_rot.toEigenMatrix4d();

	m(0, 0) *= m_sca.x;		m(0, 1) *= m_sca.y;		m(0, 2) *= m_sca.z;		m(0, 3) = m_tra.x;

	m(1, 0) *= m_sca.x;		m(1, 1) *= m_sca.y;		m(1, 2) *= m_sca.z;		m(1, 3) = m_tra.y;

	m(2, 0) *= m_sca.x;		m(2, 1) *= m_sca.y;		m(2, 2) *= m_sca.z;		m(2, 3) = m_tra.z;

	m(3, 0) = 0.0;			m(3, 1) = 0.0;			m(3, 2) = 0.0;			m(3, 3) = 1.0;

	return m;
}

QMatrix4x4 CTransform::toQMatrix4x4()
{
	QMatrix4x4 m = m_rot.toQMatrix4x4();

	m(0, 0) *= m_sca.x;		m(0, 1) *= m_sca.y;		m(0, 2) *= m_sca.z;		m(0, 3) = m_tra.x;

	m(1, 0) *= m_sca.x;		m(1, 1) *= m_sca.y;		m(1, 2) *= m_sca.z;		m(1, 3) = m_tra.y;

	m(2, 0) *= m_sca.x;		m(2, 1) *= m_sca.y;		m(2, 2) *= m_sca.z;		m(2, 3) = m_tra.z;

	m(3, 0) = 0.0;			m(3, 1) = 0.0;			m(3, 2) = 0.0;			m(3, 3) = 1.0;

	return m;
}

bool CTransform::invertMatrixD(const double m[16], double invOut[16])
{
	double inv[16], det;
	int i;

	inv[0] = m[5] * m[10] * m[15] -
		m[5] * m[11] * m[14] -
		m[9] * m[6] * m[15] +
		m[9] * m[7] * m[14] +
		m[13] * m[6] * m[11] -
		m[13] * m[7] * m[10];

	inv[4] = -m[4] * m[10] * m[15] +
		m[4] * m[11] * m[14] +
		m[8] * m[6] * m[15] -
		m[8] * m[7] * m[14] -
		m[12] * m[6] * m[11] +
		m[12] * m[7] * m[10];

	inv[8] = m[4] * m[9] * m[15] -
		m[4] * m[11] * m[13] -
		m[8] * m[5] * m[15] +
		m[8] * m[7] * m[13] +
		m[12] * m[5] * m[11] -
		m[12] * m[7] * m[9];

	inv[12] = -m[4] * m[9] * m[14] +
		m[4] * m[10] * m[13] +
		m[8] * m[5] * m[14] -
		m[8] * m[6] * m[13] -
		m[12] * m[5] * m[10] +
		m[12] * m[6] * m[9];

	inv[1] = -m[1] * m[10] * m[15] +
		m[1] * m[11] * m[14] +
		m[9] * m[2] * m[15] -
		m[9] * m[3] * m[14] -
		m[13] * m[2] * m[11] +
		m[13] * m[3] * m[10];

	inv[5] = m[0] * m[10] * m[15] -
		m[0] * m[11] * m[14] -
		m[8] * m[2] * m[15] +
		m[8] * m[3] * m[14] +
		m[12] * m[2] * m[11] -
		m[12] * m[3] * m[10];

	inv[9] = -m[0] * m[9] * m[15] +
		m[0] * m[11] * m[13] +
		m[8] * m[1] * m[15] -
		m[8] * m[3] * m[13] -
		m[12] * m[1] * m[11] +
		m[12] * m[3] * m[9];

	inv[13] = m[0] * m[9] * m[14] -
		m[0] * m[10] * m[13] -
		m[8] * m[1] * m[14] +
		m[8] * m[2] * m[13] +
		m[12] * m[1] * m[10] -
		m[12] * m[2] * m[9];

	inv[2] = m[1] * m[6] * m[15] -
		m[1] * m[7] * m[14] -
		m[5] * m[2] * m[15] +
		m[5] * m[3] * m[14] +
		m[13] * m[2] * m[7] -
		m[13] * m[3] * m[6];

	inv[6] = -m[0] * m[6] * m[15] +
		m[0] * m[7] * m[14] +
		m[4] * m[2] * m[15] -
		m[4] * m[3] * m[14] -
		m[12] * m[2] * m[7] +
		m[12] * m[3] * m[6];

	inv[10] = m[0] * m[5] * m[15] -
		m[0] * m[7] * m[13] -
		m[4] * m[1] * m[15] +
		m[4] * m[3] * m[13] +
		m[12] * m[1] * m[7] -
		m[12] * m[3] * m[5];

	inv[14] = -m[0] * m[5] * m[14] +
		m[0] * m[6] * m[13] +
		m[4] * m[1] * m[14] -
		m[4] * m[2] * m[13] -
		m[12] * m[1] * m[6] +
		m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] +
		m[1] * m[7] * m[10] +
		m[5] * m[2] * m[11] -
		m[5] * m[3] * m[10] -
		m[9] * m[2] * m[7] +
		m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] -
		m[0] * m[7] * m[10] -
		m[4] * m[2] * m[11] +
		m[4] * m[3] * m[10] +
		m[8] * m[2] * m[7] -
		m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] +
		m[0] * m[7] * m[9] +
		m[4] * m[1] * m[11] -
		m[4] * m[3] * m[9] -
		m[8] * m[1] * m[7] +
		m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] -
		m[0] * m[6] * m[9] -
		m[4] * m[1] * m[10] +
		m[4] * m[2] * m[9] +
		m[8] * m[1] * m[6] -
		m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

	if (det == 0)
		return false;

	det = 1.0 / det;

	for (i = 0; i < 16; i++)
		invOut[i] = inv[i] * det;

	return true;
}

bool CTransform::invertMatrixF(const float m[16], float invOut[16])
{
	float inv[16], det;
	int i;

	inv[0] = m[5] * m[10] * m[15] -
		m[5] * m[11] * m[14] -
		m[9] * m[6] * m[15] +
		m[9] * m[7] * m[14] +
		m[13] * m[6] * m[11] -
		m[13] * m[7] * m[10];

	inv[4] = -m[4] * m[10] * m[15] +
		m[4] * m[11] * m[14] +
		m[8] * m[6] * m[15] -
		m[8] * m[7] * m[14] -
		m[12] * m[6] * m[11] +
		m[12] * m[7] * m[10];

	inv[8] = m[4] * m[9] * m[15] -
		m[4] * m[11] * m[13] -
		m[8] * m[5] * m[15] +
		m[8] * m[7] * m[13] +
		m[12] * m[5] * m[11] -
		m[12] * m[7] * m[9];

	inv[12] = -m[4] * m[9] * m[14] +
		m[4] * m[10] * m[13] +
		m[8] * m[5] * m[14] -
		m[8] * m[6] * m[13] -
		m[12] * m[5] * m[10] +
		m[12] * m[6] * m[9];

	inv[1] = -m[1] * m[10] * m[15] +
		m[1] * m[11] * m[14] +
		m[9] * m[2] * m[15] -
		m[9] * m[3] * m[14] -
		m[13] * m[2] * m[11] +
		m[13] * m[3] * m[10];

	inv[5] = m[0] * m[10] * m[15] -
		m[0] * m[11] * m[14] -
		m[8] * m[2] * m[15] +
		m[8] * m[3] * m[14] +
		m[12] * m[2] * m[11] -
		m[12] * m[3] * m[10];

	inv[9] = -m[0] * m[9] * m[15] +
		m[0] * m[11] * m[13] +
		m[8] * m[1] * m[15] -
		m[8] * m[3] * m[13] -
		m[12] * m[1] * m[11] +
		m[12] * m[3] * m[9];

	inv[13] = m[0] * m[9] * m[14] -
		m[0] * m[10] * m[13] -
		m[8] * m[1] * m[14] +
		m[8] * m[2] * m[13] +
		m[12] * m[1] * m[10] -
		m[12] * m[2] * m[9];

	inv[2] = m[1] * m[6] * m[15] -
		m[1] * m[7] * m[14] -
		m[5] * m[2] * m[15] +
		m[5] * m[3] * m[14] +
		m[13] * m[2] * m[7] -
		m[13] * m[3] * m[6];

	inv[6] = -m[0] * m[6] * m[15] +
		m[0] * m[7] * m[14] +
		m[4] * m[2] * m[15] -
		m[4] * m[3] * m[14] -
		m[12] * m[2] * m[7] +
		m[12] * m[3] * m[6];

	inv[10] = m[0] * m[5] * m[15] -
		m[0] * m[7] * m[13] -
		m[4] * m[1] * m[15] +
		m[4] * m[3] * m[13] +
		m[12] * m[1] * m[7] -
		m[12] * m[3] * m[5];

	inv[14] = -m[0] * m[5] * m[14] +
		m[0] * m[6] * m[13] +
		m[4] * m[1] * m[14] -
		m[4] * m[2] * m[13] -
		m[12] * m[1] * m[6] +
		m[12] * m[2] * m[5];

	inv[3] = -m[1] * m[6] * m[11] +
		m[1] * m[7] * m[10] +
		m[5] * m[2] * m[11] -
		m[5] * m[3] * m[10] -
		m[9] * m[2] * m[7] +
		m[9] * m[3] * m[6];

	inv[7] = m[0] * m[6] * m[11] -
		m[0] * m[7] * m[10] -
		m[4] * m[2] * m[11] +
		m[4] * m[3] * m[10] +
		m[8] * m[2] * m[7] -
		m[8] * m[3] * m[6];

	inv[11] = -m[0] * m[5] * m[11] +
		m[0] * m[7] * m[9] +
		m[4] * m[1] * m[11] -
		m[4] * m[3] * m[9] -
		m[8] * m[1] * m[7] +
		m[8] * m[3] * m[5];

	inv[15] = m[0] * m[5] * m[10] -
		m[0] * m[6] * m[9] -
		m[4] * m[1] * m[10] +
		m[4] * m[2] * m[9] +
		m[8] * m[1] * m[6] -
		m[8] * m[2] * m[5];

	det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

	if (det == 0)
		return false;

	det = 1.0 / det;

	for (i = 0; i < 16; i++)
		invOut[i] = inv[i] * det;

	return true;
}


void CTransform::toInvertedGLMatrixD(double* matrix)
{
	double tmp[16];
	
	this->toGLMatrixD(tmp);
	invertMatrixD(tmp, matrix);
}

void CTransform::toInvertedGLMatrixF(float* matrix)
{
	float tmp[16];

	this->toGLMatrixF(tmp);
	invertMatrixF(tmp, matrix);
}

void CTransform::toInvertedRowMatrixD(double* matrix)
{
	double tmp[16];

	this->toRowMatrixD(tmp);
	invertMatrixD(tmp, matrix);
}


CPoint3d CTransform::w2l(const CPoint3d pkt)
{
	CPoint3d tmp = pkt;

	double	matrix[16];
	this->toInvertedGLMatrixD(matrix);

	return tmp.transformByMatrixD(matrix);
}

CPoint3d CTransform::l2w(const CPoint3d pkt)
{
	CPoint3d tmp = pkt;

	double	matrix[16];
	this->toGLMatrixD(matrix);

	return tmp.transformByMatrixD(matrix);
}



std::wstring CTransform::infoRow()
{
	std::wstring ret = L"Transformation: ";

	CVector3f e = this->m_rot.eulerAnglesDeg();
	ret += L"R[" + std::to_wstring( e.x ) + L"," + std::to_wstring( e.y ) + L"," + std::to_wstring( e.z ) + L"], ";

	CVector3f t = this->m_tra;
	ret += L"T[" + std::to_wstring(t.X()) + L"," + std::to_wstring(t.Y()) + L"," + std::to_wstring(t.Z()) + L"], ";

	ret += L"S[" + std::to_wstring(m_sca.x) + L"," + std::to_wstring(m_sca.y) + L"," + std::to_wstring(m_sca.z) + L"]";

	return ret;
}


QString CTransform::toString(QString prefix, QString suffix, QString separator)
{
	double matrix[16];
	
	QString text(prefix);

	this->toGLMatrixD(matrix);

	for (int col = 0; col < 4; col++)
		for (int row = 0; row < 4; row++)
		{
			int idx = 4 * row + col;

			if (idx > 0) text.append(separator);
				
			text.append( QString::number(matrix[idx]) );
		}

	text.append(suffix);
	return text;
}


void CTransform::fromRowMatrix(QString text, QString separator)
{
	if (!text.isNull())
	{
		//QRegExp sep("(\\s+)");
		QRegExp sep(separator);
		QStringList pieces = text.split(sep);

		if (pieces.length() >= 16)
		{
			double tmpMatrix[16];

			bool ok = false;
			for (int i = 0; i < 16; i++)
			{
				tmpMatrix[i] = pieces[i].toDouble(&ok);

				if (!ok) return;
			}

			this->fromRowMatrixD(tmpMatrix);
		}
	}
}


void CTransform::render()
{
	//float matrix[16];

	//toGLMatrixF(matrix);

	//glMultMatrixf(matrix);

	glMultMatrixf(toQMatrix4x4().data());
}
