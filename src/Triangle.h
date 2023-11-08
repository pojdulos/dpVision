#pragma once
#include "Global.h"

//#include <vector>
#include "BoundingBox.h"

//using namespace std;
class CMesh;

#include "Vertex.h"

class  DPVISION_EXPORT CTriangle {
public:
	union {
		//struct { CVertex m_pA, m_pB, m_pC; };
		CVertex m_pV[3];
		CVertex::Type m_v[9];
	};

	CTriangle();
	CTriangle(CPoint3d pA, CPoint3d pB, CPoint3d pC);
	CTriangle(int idx, CMesh& mesh);
	CTriangle(int idx, CMesh* mesh);
	~CTriangle();

	//static CTriangle * fromCFace(CFace &f, CMesh &mesh);

	CBoundingBox getBoundingBox();
	CPoint3d getMidpoint();

	CVector3d getNormal();
	CPoint3d getClosestPoint(const CPoint3d& sourcePosition);

	bool rayPlaneIntersect3D(CPoint3d origin, CVector3d dir, CPoint3d &pIntersectionPoint, double &pDistance);
	bool inTriangle(CPoint3d pt);
	bool hit(CPoint3d origin, CVector3d dir, CPoint3d &iP);

	template <typename Container>
	static CBoundingBox getBoundingBox(Container &container)
	{
		CBoundingBox bb( CBoundingBox::InitialValues::InvalidBB );

		for (const auto &v : container) {
			bb.expand(v->getBoundingBox());
		}

		return bb;
	}

	template <typename Container>
	static CPoint3d getMidpoint( Container &container )
	{
		CPoint3d midpt(0, 0, 0);

		for ( const auto &v : container ) {
			midpt = midpt + (v->getMidpoint() * (1.0 / container.size()));
		}
		
		return midpt;
	}
};

