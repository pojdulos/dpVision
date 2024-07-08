#include "Plane.h"

#include "Mesh.h"

void generateSquarePoints(const CPoint3d& point, const CVector3d& normal, double sideLength) {

	CVector3d arbitraryVector(1, 0, 0);
	if (normal.x == 1 && normal.y == 0 && normal.z == 0)
		arbitraryVector = CVector3d(0, 1, 0);

	CVector3d vec1 = normal.crossProduct(arbitraryVector).getNormalized() * sideLength;
	CVector3d vec2 = normal.crossProduct(vec1).getNormalized() * sideLength;

	// Generowanie punktów kwadratu
	CPoint3d p1 = point + vec1 + vec2;
	CPoint3d p2 = point + vec1 - vec2;
	CPoint3d p3 = point - vec1 - vec2;
	CPoint3d p4 = point - vec1 + vec2;

	// Wyœwietlanie punktów
	std::cout << "P1: (" << p1.x << ", " << p1.y << ", " << p1.z << ")\n";
	std::cout << "P2: (" << p2.x << ", " << p2.y << ", " << p2.z << ")\n";
	std::cout << "P3: (" << p3.x << ", " << p3.y << ", " << p3.z << ")\n";
	std::cout << "P4: (" << p4.x << ", " << p4.y << ", " << p4.z << ")\n";
}



// dzieli kwadratowe oczka na dwa trójk¹ty
CMesh* generateTriangleMesh(const CPoint3d& topLeft, const CPoint3d& topRight,
	const CPoint3d& bottomLeft, const CPoint3d& bottomRight, int divisions)
{
	CMesh* mesh = new CMesh();

	double step = 1.0 / divisions;
	// Generowanie wierzcho³ków
	for (int i = 0; i <= divisions; ++i) {
		for (int j = 0; j <= divisions; ++j) {
			CPoint3d vertex = topLeft + (topRight - topLeft) * step * j + (bottomLeft - topLeft) * step * i;
			mesh->vertices().push_back(vertex);
		}
	}

	// Generowanie indeksów
	for (int i = 0; i < divisions; ++i) {
		for (int j = 0; j < divisions; ++j) {
			int topLeftIndex = i * (divisions + 1) + j;
			int topRightIndex = topLeftIndex + 1;
			int bottomLeftIndex = (i + 1) * (divisions + 1) + j;
			int bottomRightIndex = bottomLeftIndex + 1;

			// Pierwszy trójk¹t
			mesh->faces().push_back( CFace( topLeftIndex, bottomLeftIndex, topRightIndex) );

			// Drugi trójk¹t
			mesh->faces().push_back(CFace( topRightIndex, bottomLeftIndex, bottomRightIndex) );
		}
	}

	return mesh;
}

// dieli kwadratowe oczka na cztery trójk¹ty dodajêc wierzcho³ek w œrodku
CMesh* generateTriangleMesh2(const CPoint3d& topLeft, const CPoint3d& topRight,
	const CPoint3d& bottomLeft, const CPoint3d& bottomRight, int divisions )
{
	CMesh* mesh = new CMesh();

	double step = 1.0 / divisions;

	// Generowanie wierzcho³ków
	for (int i = 0; i <= divisions; ++i) {
		for (int j = 0; j <= divisions; ++j) {
			CPoint3d vertex = topLeft + (topRight - topLeft) * step * j + (bottomLeft - topLeft) * step * i;
			mesh->addVertex(vertex);
		}
	}

	// Dodanie dodatkowych wierzcho³ków w œrodkach kwadratów
	int offset = mesh->vertices().size();
	for (int i = 0; i < divisions; ++i) {
		for (int j = 0; j < divisions; ++j) {
			CPoint3d centerVertex = (mesh->vertices()[i * (divisions + 1) + j] +
				mesh->vertices()[i * (divisions + 1) + j + 1] +
				mesh->vertices()[(i + 1) * (divisions + 1) + j] +
				mesh->vertices()[(i + 1) * (divisions + 1) + j + 1]) * 0.25;
			mesh->addVertex(centerVertex);
		}
	}

	// Generowanie indeksów
	for (int i = 0; i < divisions; ++i) {
		for (int j = 0; j < divisions; ++j) {
			int topLeftIndex = i * (divisions + 1) + j;
			int topRightIndex = topLeftIndex + 1;
			int bottomLeftIndex = (i + 1) * (divisions + 1) + j;
			int bottomRightIndex = bottomLeftIndex + 1;
			int centerIndex = offset + i * divisions + j;

			// Dzielenie kwadratu na cztery trójk¹ty
			mesh->addFace(CFace(topLeftIndex,topRightIndex,centerIndex));
			mesh->addFace(CFace(topRightIndex,bottomRightIndex,centerIndex));
			mesh->addFace(CFace(bottomRightIndex,bottomLeftIndex,centerIndex));
			mesh->addFace(CFace(bottomLeftIndex,topLeftIndex,centerIndex));
		}
	}

	return mesh;
}


CMesh* CPlane::getMesh(double size, int divX, int divY)
{
	double sideLength = 50.0;
	CPoint3d point = m_center;
	CVector3d normal = m_normal;

	CVector3d arbitraryVector(1, 0, 0);
	if (normal.x == 1 && normal.y == 0 && normal.z == 0)
		arbitraryVector = CVector3d(0, 1, 0);

	CVector3d vec1 = normal.crossProduct(arbitraryVector).getNormalized() * sideLength;
	CVector3d vec2 = normal.crossProduct(vec1).getNormalized() * sideLength;

	// Generowanie punktów kwadratu
	CPoint3d v1 = point + vec1 + vec2;
	CPoint3d v2 = point + vec1 - vec2;
	CPoint3d v3 = point - vec1 - vec2;
	CPoint3d v4 = point - vec1 + vec2;

	CMesh* plane = generateTriangleMesh2(v3, v4, v2, v1, 1000);

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

CTransform CPlane::toTransform1()
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



Eigen::Matrix3d calculateRotationMatrix(const std::vector<double>& newNormal) {
	// Wektor normalny [0,1,0]
	std::vector<double> defaultNormal = { 0, 0, 1 };

	// Obliczenie osi rotacji (iloczyn wektorowy)
	std::vector<double> rotationAxis = {
		defaultNormal[1] * newNormal[2] - defaultNormal[2] * newNormal[1],
		defaultNormal[2] * newNormal[0] - defaultNormal[0] * newNormal[2],
		defaultNormal[0] * newNormal[1] - defaultNormal[1] * newNormal[0]
	};

	// Obliczenie k¹ta rotacji (iloczyn skalarny i arccos)
	double dotProduct = defaultNormal[1] * newNormal[1]; // Tylko sk³adowe Y, inne s¹ zerowe
	double angle = acos(dotProduct);

	// Normalizacja osi rotacji
	double norm = sqrt(rotationAxis[0] * rotationAxis[0] +
		rotationAxis[1] * rotationAxis[1] +
		rotationAxis[2] * rotationAxis[2]);
	rotationAxis[0] /= norm;
	rotationAxis[1] /= norm;
	rotationAxis[2] /= norm;

	// Sk³adniki macierzy skoœnosymetrycznej
	double Kx = rotationAxis[0], Ky = rotationAxis[1], Kz = rotationAxis[2];
	Eigen::Matrix3d K;
	K(0, 1) = -Kz; K(0, 2) = Ky;
	K(1, 0) = Kz;  K(1, 2) = -Kx;
	K(2,0) = -Ky; K(2,1) = Kx;

	// Obliczenie macierzy rotacji
	Eigen::Matrix3d R;
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			R(i,j) = 0;
			for (int k = 0; k < 3; ++k) {
				R(i,j) += (sin(angle) * K(i,k) + (1 - cos(angle)) * K(i,k) * K(k,j));
			}
			if (i == j) {
				R(i,j) += 1;
			}
		}
	}

	return R;
}

CTransform CPlane::toTransform()
{
	Eigen::Matrix4d mat4d = Eigen::Matrix4d::Identity();
	mat4d.block<3, 3>(0, 0) = calculateRotationMatrix({ m_normal.x, m_normal.y, m_normal.z });

	return CTransform(mat4d);
}