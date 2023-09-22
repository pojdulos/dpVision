#include "Triangle.h"

#include "Mesh.h"

//using namespace std;

CTriangle::~CTriangle() {}

CTriangle::CTriangle() {}

CTriangle::CTriangle(CPoint3d pA, CPoint3d pB, CPoint3d pC)
{
	m_pV[0] = pA;
	m_pV[1] = pB;
	m_pV[2] = pC;
}

CTriangle::CTriangle(int idx, CMesh* mesh)
{
	m_pV[0] = mesh->vertices()[mesh->faces()[idx].A()];
	m_pV[1] = mesh->vertices()[mesh->faces()[idx].B()];
	m_pV[2] = mesh->vertices()[mesh->faces()[idx].C()];
}

CTriangle::CTriangle(int idx, CMesh& mesh)
{
	m_pV[0] = mesh.vertices()[mesh.faces()[idx].A()];
	m_pV[1] = mesh.vertices()[mesh.faces()[idx].B()];
	m_pV[2] = mesh.vertices()[mesh.faces()[idx].C()];
}

//static CTriangle * CTriangle::fromCFace(CFace &f, CMesh &mesh)
//{
//	CTriangle * t = new CTriangle();

//	t->m_iA = f.A();
//	t->m_iB = f.B();
//	t->m_iC = f.C();

//	t->m_pA = mesh.vertices()[t->m_iA];
//	t->m_pB = mesh.vertices()[t->m_iB];
//	t->m_pC = mesh.vertices()[t->m_iC];

//	t->m_idx = idx;

//	return t;
//}

CBoundingBox CTriangle::getBoundingBox()
{
	return CBoundingBox(m_pV[0], m_pV[1], m_pV[2]);
}


CPoint3d CTriangle::getMidpoint()
{
	return CBoundingBox(m_pV[0], m_pV[1], m_pV[2]).getMidpoint();
}


bool CTriangle::rayPlaneIntersect3D(CPoint3d origin, CVector3d dir, CPoint3d &pIntersectionPoint, double &pDistance)
{
	CVector3d vNorm = CVector3d(m_pV[0], m_pV[1]).crossProduct(CVector3d(m_pV[0], m_pV[2])).getNormalized();

	if (0 == vNorm.length()) return false;

	// wektor od wierzcho³ka trójk¹ta do punktu na promieniu
	CVector3d w0 = CVector3d(m_pV[0], origin);

	// iloczyn skalarny -> zero jeœli wektory prostopad³e
	double a = -vNorm.dotProduct(w0); // 0 -> w0 prostopad³y do vNorm -> punkt pP0 le¿y na p³aszczyŸnie trójk¹ta

	if (a == 0)
	{
		pIntersectionPoint = origin;
		pDistance = 0.0;

		return true;
	}

	double b = vNorm.dotProduct(dir);	// b == 0 -> vRay prostopad³y do vNorm -> vRay jest równoleg³y do trójk¹ta
								// b < 0  -> vRay wpada od przodu -> OK
								// b > 0  -> vRay wpada od ty³u -> NIE OK

	//if ( fabs(b) < prawieZero ) // to jest chyba niepotrzebne o ile sie nie pojawi¹ b³êdy
	if (b == 0)
	{ // vRay jest równoleg³y do p³aszczyzny trójk¹ta
		return false;
	}

	double r = a / b;

	// Wyznaczam punkt przeciêcia promienia z p³aszczyzna œciany
	CVector3d vec = (dir * a) / b;

	pDistance = abs(r);
	pIntersectionPoint = origin + vec;

	return true;
}


bool CTriangle::inTriangle(CPoint3d pt)
{
	CVector3d u = CVector3d(m_pV[0], m_pV[1]);
	CVector3d v = CVector3d(m_pV[0], m_pV[2]);

	// --------------------------------------------------------------
	// sprawdzam czy pt lezy w trójk¹cie tFace
	// --------------------------------------------------------------
	// to jest wyznaczane ze wspó³rzêdnych barycentrycznych

	double uu = u.dotProduct(u);
	double uv = u.dotProduct(v);
	double vv = v.dotProduct(v);

	CVector3d w = CVector3d(m_pV[0], pt);

	double wu = w.dotProduct(u);
	double wv = w.dotProduct(v);

	double D = uv * uv - uu * vv;

	// get and test parametric coords
	double s = (uv * wv - vv * wu) / D;

	if (s < 0.0 || s > 1.0)         // pt le¿y poza trójk¹tem
		return false;

	double t = (uv * wu - uu * wv) / D;

	if (t < 0.0 || (s + t) > 1.0)  // pt le¿y poza trójk¹tem
		return false;

	return true; // pt le¿y na trójk¹cie
}


bool CTriangle::hit( CPoint3d origin, CVector3d dir, CPoint3d &iP )
{
	CPoint3d pIntersectionPoint;
	double pDistance;

	if (rayPlaneIntersect3D(origin, dir, pIntersectionPoint, pDistance))
	{
		iP = pIntersectionPoint;
		return inTriangle(pIntersectionPoint);
	}
		
	return false;
}

