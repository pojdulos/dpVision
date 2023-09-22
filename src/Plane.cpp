#include "Plane.h"

#include "Mesh.h"

CMesh* CPlane::getMesh(double size, int divX, int divY)
{
	// wzór wyjœciowy: n.x*v.x + n.y*v.y +n.z*v.z = 0
	// v.x = ( n.y*v.y + n.z*v.z ) / -n.x
	// v.y = ( n.x*v.x + n.z*v.z ) / -n.y
	// v.z = ( n.x*v.x + n.y*v.y ) / -n.z

	// przyjmujê wstêpnie wartoœci v.x, v.y oraz v.z = 1.0 dla uproszczenia obliczeñ
	double x = 1.0;
	double y = 1.0;
	double z = 1.0;

	// mianownik nie mo¿e byæ zerowy
	if (m_normal.Z() != 0.0) {
		z = (m_normal.X() + m_normal.Y()) / -m_normal.Z();
	}
	else if (m_normal.Y() != 0.0) {
		y = (m_normal.X() + m_normal.Z()) / -m_normal.Y();
	}
	else {
		x = (m_normal.Y() + m_normal.Z()) / -m_normal.X();
	}


	CVector3d v1(CVector3d(x, y, z).getNormalized() * size);			// mamy wektor kierunkowy do pierwszego naro¿nika, który mno¿ymy od razu przez m_size
	CVector3d v2(v1.crossProduct(m_normal).getNormalized() * size);	// wektor prostopad³y jednoczeœnie do normalnego i do v1 wyznaczy drugi naro¿nik
	CVector3d v3(-v1);													// wektor przeciwny do v1 wyznacza trzeci naro¿nik,
	CVector3d v4(-v2);													// a wektor przeciwny do v2 - czwarty


	CMesh* plane = new CMesh();

	plane->addVertex(v1);
	plane->addVertex(v2);
	plane->addVertex(v3);
	plane->addVertex(v4);

	plane->faces().push_back(CFace(0, 1, 2));
	plane->faces().push_back(CFace(0, 2, 3));

	plane->fnormals().push_back(CFace(0, 1, 2).getNormal(plane->vertices()));
	plane->fnormals().push_back(CFace(2, 3, 0).getNormal(plane->vertices()));

	plane->getMaterial().FrontColor.ambient.SetFloat(0.0f, 1.0f, 1.0f, 0.8f);
	plane->getMaterial().FrontColor.diffuse.SetFloat(0.0f, 1.0f, 1.0f, 0.8f);

	plane->getMaterial().BackColor.ambient.SetFloat(1.0f, 0.0f, 0.0f, 1.0f);
	plane->getMaterial().BackColor.diffuse.SetFloat(1.0f, 0.0f, 0.0f, 1.0f);
	plane->getMaterial().BackColor.specular.SetFloat(0.5f, 0.0f, 0.0f, 1.0f);
	plane->getMaterial().BackColor.emission.SetFloat(0.5f, 0.0f, 0.0f, 1.0f);
	plane->getMaterial().BackColor.shininess = 1.0f;


	return plane;
}


void CPlane::toMatrix( double *m )
{
	m[0] = 1.0;
	m[1] = 0.0;
	m[2] = 0.0;
	m[3] = m_center.x;

	m[4] = 0.0;
	m[5] = m_normal.z;
	m[6] = m_normal.y;
	m[7] = m_center.y;

	m[8] = 0.0;
	m[9] = -m_normal.y;
	m[10] = m_normal.z;
	m[11] = m_center.z;

	m[12] = 0.0;
	m[13] = 0.0;
	m[14] = 0.0;
	m[15] = 1.0;
}

CTransform CPlane::toTransform()
{
//	double Rx[9], Ry[9], Rz[9];

	long double cosG = m_normal.z;
	long double sinG = m_normal.y;

//	Rx[0] = 1.0;		Rx[1] = 0.0;		Rx[2] = 0.0;
//	Rx[3] = 0.0;		Rx[4] = cosG;		Rx[5] = -sinG;
//	Rx[6] = 0.0;		Rx[7] = sinG;		Rx[8] = cosG;


	long double cosB = m_normal.z;
	long double sinB = -m_normal.x;

//	Ry[0] = cosB;		Ry[1] = 0.0;		Ry[2] = sinB;
//	Ry[3] = 0.0;		Ry[4] = 1.0;		Ry[5] = 0.0;
//	Ry[6] = -sinB;		Ry[7] = 0.0;		Ry[8] = cosB;

	long double cosA = m_normal.y;
	long double sinA = m_normal.x;

//	Rz[0] = cosA;		Rz[1] = -sinA;		Rz[2] = 0.0;
//	Rz[3] = sinA;		Rz[4] = cosA;		Rz[5] = 0.0;
//	Rz[6] = 0.0;		Rz[7] = 0.0;		Rz[8] = 1.0;

	//t.rotateByMatrix3x3(Rx);
	//t.rotateByMatrix3x3(Ry);
	//t.rotateByMatrix3x3(Rz);

	double R[9];

	R[0] = cosA*cosB;		R[1] = cosA*sinB*sinG-sinA*cosG;	R[2] = cosA*sinB*cosG+sinA*sinG;
	R[3] = sinA*cosB;		R[4] = sinA*sinB*sinG+cosA*cosG;	R[5] = sinA*sinB*cosG-cosA*sinG;
	R[6] = -sinB;			R[7] = cosB*sinG;					R[8] = cosB*cosG;


	CTransform t;
	
	t.setOrigin();
	t.rotation().setIdentity();
	
	t.rotateByMatrix3x3(R);

	t.translation() = CVector3d(CPoint3d(0.0, 0.0, 0.0), m_center);
	t.setScale(1.0);
	return t;
}