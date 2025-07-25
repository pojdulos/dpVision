#pragma once
#include "Global.h"

//#include <vector>
#include "BoundingBox.h"

//using namespace std;
class CMesh;

#include "Vertex.h"

class DPVISION_EXPORT CTriangle {
public:
	CVertex a, b, c;

	CTriangle() = default;
	CTriangle(CPoint3d pA, CPoint3d pB, CPoint3d pC);
	CTriangle(int idx, CMesh& mesh);
	CTriangle(int idx, CMesh* mesh);

	~CTriangle() = default;

	CVertex& operator[](size_t i) {
		switch (i) {
		case 0: return a;
		case 1: return b;
		case 2: return c;
		default: throw std::out_of_range("CTriangle: index out of range");
		}
	}

	const CVertex& operator[](size_t i) const {
		switch (i) {
		case 0: return a;
		case 1: return b;
		case 2: return c;
		default: throw std::out_of_range("CTriangle: index out of range");
		}
	}
	//static CTriangle * fromCFace(CFace &f, CMesh &mesh);

	CBoundingBox getBoundingBox() const;
	CPoint3d getMidpoint() const;

	CVector3d getNormal() const;

	static CPoint3d getClosestPoint(const CPoint3d& sourcePosition, const CVertex& _a, const CVertex& _b, const CVertex& _c);
	inline CPoint3d getClosestPoint(const CPoint3d& sourcePosition) {
		return CTriangle::getClosestPoint(sourcePosition, this->a, this->b, this->c);
	};

	bool rayPlaneIntersect3D(CPoint3d origin, CVector3d dir, CPoint3d &pIntersectionPoint, double &pDistance);
	bool inTriangle(CPoint3d pt);
	bool hit(CPoint3d origin, CVector3d dir, CPoint3d &iP);

	CTriangle transformByMatrix(Eigen::Matrix4d matrix);
	CTriangle transformByMatrix(Eigen::Matrix3d matrix);

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

