#include "Triangle.h"

#include "Mesh.h"

//using namespace std;

CTriangle::~CTriangle() {}

CTriangle::CTriangle() {}

CTriangle::CTriangle(CPoint3d pA, CPoint3d pB, CPoint3d pC)
{
	a = pA;
	b = pB;
	c = pC;
}

CTriangle::CTriangle(int idx, CMesh* mesh)
{
	a = mesh->vertices()[mesh->faces()[idx].A()];
	b = mesh->vertices()[mesh->faces()[idx].B()];
	c = mesh->vertices()[mesh->faces()[idx].C()];
}

CTriangle::CTriangle(int idx, CMesh& mesh)
{
	a = mesh.vertices()[mesh.faces()[idx].A()];
	b = mesh.vertices()[mesh.faces()[idx].B()];
	c = mesh.vertices()[mesh.faces()[idx].C()];
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
	return CBoundingBox(a, b, c);
}


CPoint3d CTriangle::getMidpoint()
{
	return CBoundingBox(a, b, c).getMidpoint();
}


bool CTriangle::rayPlaneIntersect3D(CPoint3d origin, CVector3d dir, CPoint3d &pIntersectionPoint, double &pDistance)
{
	CVector3d vNorm = CVector3d(a, b).crossProduct(CVector3d(a, c)).getNormalized();

	if (0 == vNorm.length()) return false;

	// wektor od wierzcho�ka tr�jk�ta do punktu na promieniu
	CVector3d w0 = CVector3d(a, origin);

	// iloczyn skalarny -> zero je�li wektory prostopad�e
	double a = -vNorm.dotProduct(w0); // 0 -> w0 prostopad�y do vNorm -> punkt pP0 le�y na p�aszczy�nie tr�jk�ta

	if (abs(a) < 0.00000001)
	{
		pIntersectionPoint = origin;
		pDistance = 0.0;

		return true;
	}

	double b = vNorm.dotProduct(dir);	// b == 0 -> vRay prostopad�y do vNorm -> vRay jest r�wnoleg�y do tr�jk�ta
								// b < 0  -> vRay wpada od przodu -> OK
								// b > 0  -> vRay wpada od ty�u -> NIE OK

	//if ( fabs(b) < prawieZero ) // to jest chyba niepotrzebne o ile sie nie pojawi� b��dy
	if (b == 0)
	{ // vRay jest r�wnoleg�y do p�aszczyzny tr�jk�ta
		return false;
	}

	double r = a / b;

	// Wyznaczam punkt przeci�cia promienia z p�aszczyzna �ciany
	CVector3d vec = (dir * a) / b;

	pDistance = abs(r);
	pIntersectionPoint = origin + vec;

	return true;
}


bool CTriangle::inTriangle(CPoint3d pt)
{
	CVector3d u = CVector3d(a, b);
	CVector3d v = CVector3d(a, c);

	// --------------------------------------------------------------
	// sprawdzam czy pt lezy w tr�jk�cie tFace
	// --------------------------------------------------------------
	// to jest wyznaczane ze wsp�rz�dnych barycentrycznych

	double uu = u.dotProduct(u);
	double uv = u.dotProduct(v);
	double vv = v.dotProduct(v);

	CVector3d w = CVector3d(a, pt);

	double wu = w.dotProduct(u);
	double wv = w.dotProduct(v);

	double D = uv * uv - uu * vv;

	// get and test parametric coords
	double s = (uv * wv - vv * wu) / D;

	if (s < 0.0 || s > 1.0)         // pt le�y poza tr�jk�tem
		return false;

	double t = (uv * wu - uu * wv) / D;

	if (t < 0.0 || (s + t) > 1.0)  // pt le�y poza tr�jk�tem
		return false;

	return true; // pt le�y na tr�jk�cie
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

CTriangle CTriangle::transformByMatrix(Eigen::Matrix4d matrix)
{
	CTriangle result;

	result.a = this->a.transformByMatrix(matrix);
	result.b = this->b.transformByMatrix(matrix);
	result.c = this->c.transformByMatrix(matrix);
	return result;
}

CTriangle CTriangle::transformByMatrix(Eigen::Matrix3d matrix)
{
	CTriangle result;

	result.a = this->a.transformByMatrix(matrix);
	result.b = this->b.transformByMatrix(matrix);
	result.c = this->c.transformByMatrix(matrix);
	return result;
}

CVector3d CTriangle::getNormal()
{
	// wektor f.A->f.B
	CVector3d v1(a, b);

	// wektor f.A->f.C
	CVector3d v2(a, c);

	// iloczyn wektorowy
	CVector3d vn = v1.crossProduct(v2);

	// Normalizacja
	vn.normalize();

	return (vn);
}


//
//template<class T, class Compare>
//constexpr const T& clamp(const T& v, const T& lo, const T& hi, Compare comp)
//{
//	return assert(!comp(hi, lo)),
//		comp(v, lo) ? lo : comp(hi, v) ? hi : v;
//}
//
//template<class T>
//constexpr const T& clamp(const T& v, const T& lo, const T& hi)
//{
//	return clamp(v, lo, hi, std::less<>());
//}

#include <algorithm>

using std::clamp;

CPoint3d CTriangle::getClosestPoint(const CPoint3d& sourcePosition, const CVertex& _a, const CVertex& _b, const CVertex& _c )
{
	CVector3d edge0(_a, _b);
	CVector3d edge1(_a, _c);
	CVector3d v0(sourcePosition, _a);

	float a = edge0.dotProduct(edge0);
	float b = edge0.dotProduct(edge1);
	float c = edge1.dotProduct(edge1);
	float d = edge0.dotProduct(v0);
	float e = edge1.dotProduct(v0);

	float det = a * c - b * b;
	float s = b * e - c * d;
	float t = b * d - a * e;

	if (s + t < det)
	{
		if (s < 0.f)
		{
			if (t < 0.f)
			{
				if (d < 0.f)
				{
					s = clamp(-d / a, 0.f, 1.f);
					t = 0.f;
				}
				else
				{
					s = 0.f;
					t = clamp(-e / c, 0.f, 1.f);
				}
			}
			else
			{
				s = 0.f;
				t = clamp(-e / c, 0.f, 1.f);
			}
		}
		else if (t < 0.f)
		{
			s = clamp(-d / a, 0.f, 1.f);
			t = 0.f;
		}
		else
		{
			float invDet = 1.f / det;
			s *= invDet;
			t *= invDet;
		}
	}
	else
	{
		if (s < 0.f)
		{
			float tmp0 = b + d;
			float tmp1 = c + e;
			if (tmp1 > tmp0)
			{
				float numer = tmp1 - tmp0;
				float denom = a - 2 * b + c;
				s = clamp(numer / denom, 0.f, 1.f);
				t = 1 - s;
			}
			else
			{
				t = clamp(-e / c, 0.f, 1.f);
				s = 0.f;
			}
		}
		else if (t < 0.f)
		{
			if (a + d > b + e)
			{
				float numer = c + e - b - d;
				float denom = a - 2 * b + c;
				s = clamp(numer / denom, 0.f, 1.f);
				t = 1 - s;
			}
			else
			{
				s = clamp(-e / c, 0.f, 1.f);
				t = 0.f;
			}
		}
		else
		{
			float numer = c + e - b - d;
			float denom = a - 2 * b + c;
			s = clamp(numer / denom, 0.f, 1.f);
			t = 1.f - s;
		}
	}

	return _a + edge0 * s + edge1 * t;
}

//CPoint3d CTriangle::getClosestPoint(const CPoint3d & sourcePosition)
//{
//	CVector3d edge0(a, b);
//	CVector3d edge1(a, c);
//	CVector3d v0(sourcePosition, a);
//
//	float a = edge0.dotProduct(edge0);
//	float b = edge0.dotProduct(edge1);
//	float c = edge1.dotProduct(edge1);
//	float d = edge0.dotProduct(v0);
//	float e = edge1.dotProduct(v0);
//
//	float det = a * c - b * b;
//	float s = b * e - c * d;
//	float t = b * d - a * e;
//
//	if (s + t < det)
//	{
//		if (s < 0.f)
//		{
//			if (t < 0.f)
//			{
//				if (d < 0.f)
//				{
//					s = clamp(-d / a, 0.f, 1.f);
//					t = 0.f;
//				}
//				else
//				{
//					s = 0.f;
//					t = clamp(-e / c, 0.f, 1.f);
//				}
//			}
//			else
//			{
//				s = 0.f;
//				t = clamp(-e / c, 0.f, 1.f);
//			}
//		}
//		else if (t < 0.f)
//		{
//			s = clamp(-d / a, 0.f, 1.f);
//			t = 0.f;
//		}
//		else
//		{
//			float invDet = 1.f / det;
//			s *= invDet;
//			t *= invDet;
//		}
//	}
//	else
//	{
//		if (s < 0.f)
//		{
//			float tmp0 = b + d;
//			float tmp1 = c + e;
//			if (tmp1 > tmp0)
//			{
//				float numer = tmp1 - tmp0;
//				float denom = a - 2 * b + c;
//				s = clamp(numer / denom, 0.f, 1.f);
//				t = 1 - s;
//			}
//			else
//			{
//				t = clamp(-e / c, 0.f, 1.f);
//				s = 0.f;
//			}
//		}
//		else if (t < 0.f)
//		{
//			if (a + d > b + e)
//			{
//				float numer = c + e - b - d;
//				float denom = a - 2 * b + c;
//				s = clamp(numer / denom, 0.f, 1.f);
//				t = 1 - s;
//			}
//			else
//			{
//				s = clamp(-e / c, 0.f, 1.f);
//				t = 0.f;
//			}
//		}
//		else
//		{
//			float numer = c + e - b - d;
//			float denom = a - 2 * b + c;
//			s = clamp(numer / denom, 0.f, 1.f);
//			t = 1.f - s;
//		}
//	}
//
//	return this->a + edge0 * s + edge1 * t;
//}
