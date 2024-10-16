#include "KDNode.h"
#include "IndexedTriangle.h"

KDNode::~KDNode()
{
	if (left) delete left;
	if (right) delete right;
}


KDNode * KDNode::build2(vector<CIndexedTriangle*> &tris, int depth)
{
	KDNode *node = new KDNode();
	//node->triangles = tris;
	node->triangles.clear();
	node->bbox = CBoundingBox();

	if (tris.size() == 0) return node;

	if (tris.size() == 1) {
		node->bbox = tris[0]->getBoundingBox();

		node->left = new KDNode();
		node->left->triangles = std::vector<CIndexedTriangle*>();

		node->right = new KDNode();
		node->right->triangles = std::vector<CIndexedTriangle*>();

		return node;
	}

	//node->bbox = tris[0]->getBoundingBox();

	//for (int i = 1; i < tris.size(); i++) {
	//	node->bbox.expand(tris[i]->getBoundingBox());
	//}

	node->bbox = CIndexedTriangle::getBoundingBox( tris );

	if (tris.size() > 128)
	{
		//CPoint3d midpt(0, 0, 0);

		//for (int i = 0; i < tris.size(); i++) {
		//	midpt = midpt + (tris[i]->getMidpoint() * (1.0 / tris.size()));
		//}

		CPoint3d midpt = CIndexedTriangle::getMidpoint( tris );

		vector<CIndexedTriangle*> left_tris;
		vector<CIndexedTriangle*> right_tris;

		int axis = node->bbox.longest_axis();

		for (int i = 0; i < tris.size(); i++) {
			switch (axis) {
			case 0:
				midpt.x >= tris[i]->getMidpoint().x ? right_tris.push_back(tris[i]) : left_tris.push_back(tris[i]);
				break;
			case 1:
				midpt.y >= tris[i]->getMidpoint().y ? right_tris.push_back(tris[i]) : left_tris.push_back(tris[i]);
				break;
			case 2:
				midpt.z >= tris[i]->getMidpoint().z ? right_tris.push_back(tris[i]) : left_tris.push_back(tris[i]);
				break;
			}
		}

		node->left = KDNode::build(left_tris, depth + 1);
		node->right = KDNode::build(right_tris, depth + 1);
	}
	else
	{
		node->triangles = tris;

		node->left = new KDNode();
		node->left->triangles = vector<CIndexedTriangle*>();

		node->right = new KDNode();
		node->right->triangles = vector<CIndexedTriangle*>();
	}

	return node;
}

KDNode * KDNode::build(vector<CIndexedTriangle*> &tris, int depth)
{
	KDNode *node = new KDNode();
	node->triangles = tris;
	node->bbox = CBoundingBox();

	if (tris.size() == 0) return node;

	if (tris.size() == 1) {
		node->bbox = tris[0]->getBoundingBox();
		node->left = new KDNode();
		node->right = new KDNode();
		node->left->triangles = std::vector<CIndexedTriangle*>();
		node->right->triangles = std::vector<CIndexedTriangle*>();
		return node;
	}

	node->bbox = tris[0]->getBoundingBox();

	for (int i = 1; i < tris.size(); i++) {
		node->bbox.expand(tris[i]->getBoundingBox());
	}

	CPoint3d midpt(0, 0, 0);

	for (int i = 0; i < tris.size(); i++) {
		midpt = midpt + (tris[i]->getMidpoint() * (1.0 / tris.size()));
	}

	vector<CIndexedTriangle*> left_tris;
	vector<CIndexedTriangle*> right_tris;

	int axis = node->bbox.longest_axis();

	for (int i = 0; i < tris.size(); i++) {
		switch (axis) {
		case 0:
			midpt.x >= tris[i]->getMidpoint().x ? right_tris.push_back(tris[i]) : left_tris.push_back(tris[i]);
			break;
		case 1:
			midpt.y >= tris[i]->getMidpoint().y ? right_tris.push_back(tris[i]) : left_tris.push_back(tris[i]);
			break;
		case 2:
			midpt.z >= tris[i]->getMidpoint().z ? right_tris.push_back(tris[i]) : left_tris.push_back(tris[i]);
			break;
		}
	}

	int matches = 0;

	if (left_tris.size() == 0 && right_tris.size() > 0)
	{
		left_tris = right_tris;
		matches = right_tris.size();
	}
	else if (right_tris.size() == 0 && left_tris.size() > 0)
	{
		right_tris = left_tris;
		matches = left_tris.size();
	}
	else
	{
		for (int i = 0; i < left_tris.size(); i++)
		{
			for (int j = 0; j < right_tris.size(); j++)
			{
				if (left_tris[i] == right_tris[j])
					matches++;
			}
		}
	}

	if ((float)matches / left_tris.size() < 0.5 && (float)matches / right_tris.size() < 0.5) {
		node->left = KDNode::build(left_tris, depth + 1);
		node->right = KDNode::build(right_tris, depth + 1);
	}
	else {
		node->left = new KDNode();
		node->left->triangles = vector<CIndexedTriangle*>();

		node->right = new KDNode();
		node->right->triangles = vector<CIndexedTriangle*>();
	}

	return node;
}


bool KDNode::hit(const CPoint3d origin, const CVector3d dir, ShadeRec &sr)
{
	CPoint3d tmp;
	if ( bbox.hit(origin, dir, tmp) )
	{
		bool hit_tri = false;

		if (triangles.size() > 0)
		{
			for (int i = 0; i < triangles.size(); i++)
			{
				CPoint3d iP;
				if (triangles[i]->hit(origin, dir, iP)) {
					if ((sr.vidx != triangles[i]->m_iA) && (sr.vidx != triangles[i]->m_iB) && (sr.vidx != triangles[i]->m_iC))
					{
						if (dir.dotProduct(CVector3d(origin, iP)) > 0)
						{
							hit_tri = true;
							sr.fidxs[triangles[i]->m_idx] = std::pair<double, CPoint3d>(dir.dotProduct(CVector3d(origin, iP)), iP);
						}
					}
				}
			}

			if (hit_tri && (sr.fidxs.size() > 0)) {
				return true;
			}

			return false;
		}
		else //			if ( ( left->triangles.size() > 0 ) || ( right->triangles.size() > 0 ) )
		{
			bool hitleft = left->hit(origin, dir, sr);
			bool hitright = right->hit(origin, dir, sr);
			return ( hitleft || hitright );
		}
	}
	return false;
}

