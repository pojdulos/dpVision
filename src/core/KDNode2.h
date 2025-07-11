#pragma once
#include "Global.h"
#include <vector>
#include <set>

#include "BoundingBox.h"

//using namespace std;

class CMesh;

class  DPVISION_EXPORT KDNode2 {
public:
	typedef std::map<int, std::pair<double, CPoint3d>> HitMap;

	typedef struct {
		int vidx;
		HitMap fidxs;
	} ShadeRec;

	CBoundingBox m_bbox;
	KDNode2 *m_left;
	KDNode2 *m_right;
	std::vector<INDEX_TYPE> m_tris;

	KDNode2() :m_left{ NULL }, m_right{ NULL }, m_tris( std::vector<INDEX_TYPE>(0) ), m_bbox( CBoundingBox(CBoundingBox::InvalidBB) ) {};
	~KDNode2();

	static CBoundingBox createBB(CMesh* mesh, INDEX_TYPE face_idxi);

	static CBoundingBox createBB(CMesh* mesh, std::vector<INDEX_TYPE> tris);

	static KDNode2* build(CMesh* mesh, int maxTrisSize = 256);
	static KDNode2* build2(CMesh* mesh, std::vector<INDEX_TYPE>& tris, int depth, int maxTrisSize);
	static KDNode2* build(CMesh* mesh, std::vector<INDEX_TYPE>& tris, int depth, int maxTrisSize = 256);

	bool hit(CMesh *mesh, const CPoint3d origin, const CVector3d dir, ShadeRec &sr);
	bool hit(CMesh* mesh, const CPoint3d origin, const CVector3d dir, HitMap& hmap, long vidx = -1);

	bool findCrossedBB(CBoundingBox bb, CMesh* mesh, std::set<INDEX_TYPE> &result);
};

