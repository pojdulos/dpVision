#pragma once

#include <vector>

#include "BoundingBox.h"
#include "Mesh.h"

using namespace std;

class CIndexedTriangle;

class  DPVISION_EXPORT KDNode {
public:
	typedef struct {
		int vidx;
		map<int, std::pair<double, CPoint3d>> fidxs;
	} ShadeRec;

	CBoundingBox bbox;
	KDNode *left;
	KDNode *right;
	vector<CIndexedTriangle*> triangles;

	KDNode() :left{ NULL }, right{ NULL } {};
	~KDNode();

	static KDNode * build(vector<CIndexedTriangle*> &tris, int depth);
	static KDNode * build2(vector<CIndexedTriangle*> &tris, int depth);

	bool hit(const CPoint3d origin, const CVector3d dir, ShadeRec &sr);
};

