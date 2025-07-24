#include "Face.h"

void CFace::invert()
{
	INDEX_TYPE tmp = y;
	y = z;
	z = tmp;
}


CVector3d CFace::getNormal(const std::vector<CVertex> &vertices)
{
	CVector3d v1, v2;
	CVector3d vn;

	if (MAX3(x, y, z) < vertices.size())
	{
		// wektor f.A->f.B
		CVector3d v1(vertices[x], vertices[y]);

		// wektor f.A->f.C
		CVector3d v2(vertices[x], vertices[z]);

		// iloczyn wektorowy
		vn = v1.crossProduct(v2);

		// Normalizacja
		vn.normalize();
	}

	return (vn);
}

CVector3d CFace::triangleNormal(CPoint3d pA, CPoint3d pB, CPoint3d pC)
{
	return CVector3d(pA, pB).crossProduct(CVector3d(pA, pC)).getNormalized();
}

double CFace::triangleArea(CPoint3d pA, CPoint3d pB, CPoint3d pC)
{
	return 0.5*CVector3d(pB, pA).crossProduct(CVector3d(pC, pA)).length();
}

int CFace::hasVertex( const INDEX_TYPE v_id )
{
	if (x==v_id) return 0;
	else if (y==v_id) return 1;
	else if (z==v_id) return 2;

	return -1;
}

void CFace::setAt(int n, const INDEX_TYPE v_id)
{
	switch (n) {
		case 0: x = v_id; break;
		case 1: y = v_id; break;
		case 2: z = v_id; break;
		default: break;
	}
}

INDEX_TYPE CFace::at(int n)
{
	switch (n) {
		case 0: return x; break;
		case 1: return y; break;
		case 2: return z; break;
		default: return -1; break;
	}
}
