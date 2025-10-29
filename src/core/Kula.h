#pragma once
//class CPoint3d;
#include "Point3.h"
#include "Vertex.h"
#include "Face.h"


class DPVISION_EXPORT Kula
{
protected:
	CPoint3d m_c;
	double m_r;

public:
	Kula() : m_c(0.0, 0.0, 0.0), m_r(1.0) {};
	Kula(double x, double y, double z, double r) : m_c(x,y,z), m_r(r) {};
	Kula(CPoint3d c, double r) : m_c(c), m_r(r) {};
	Kula(const Kula &k) : m_c(k.m_c), m_r(k.m_r) {};
	Kula(Kula& k) : m_c(k.m_c), m_r(k.m_r) {};
	~Kula() {};

	CPoint3d& center() { return m_c; };
	double& radius() { return m_r; };

	bool isValid() { return m_r > 0; };

	static Kula* fromTriangle(CPoint3d A, CPoint3d B, CPoint3d C);
	inline static Kula* fromTriangle(CPoint3d v[3]) { return Kula::fromTriangle(v[0], v[1], v[2]); };


	static Kula* fromPoints(std::vector<CVertex> pts);
	static Kula* fromPoints2(std::vector<CVertex> &pts);
	static Kula* fromPoints3(std::vector<CVertex>& pts);

	static double findMaxDist_BruteForce(std::vector<CVertex>& pts, std::pair<INDEX_TYPE, INDEX_TYPE>& result);

	void findBiggestTriangle(std::vector<CVertex>& pts, INDEX_TYPE result[3]);

	static Kula* fromMesh(std::vector<CFace> &fcs, std::vector<CVertex> &pts);

	static Kula* bound(Kula k1, Kula k2);
	
	void bound2(Kula k1, Kula k2);

	void scal(Kula k2);

	bool contains(CPoint3d p) { return (m_r >= m_c.distanceTo(p)); }
	bool contains(Kula k) { return m_r >= (m_c.distanceTo(k.m_c) + k.m_r); }
};

