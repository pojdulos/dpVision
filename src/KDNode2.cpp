#include "KDNode2.h"
#include "Mesh.h"

KDNode2::~KDNode2()
{
	if (m_left) delete m_left;
	if (m_right) delete m_right;
}


CBoundingBox KDNode2::createBB(CMesh* mesh, INDEX_TYPE face_idx)
{
	return CBoundingBox(mesh->vertex(face_idx, 0), mesh->vertex(face_idx, 1), mesh->vertex(face_idx, 2));
}

CBoundingBox KDNode2::createBB(CMesh* mesh, std::vector<INDEX_TYPE> tris)
{
	CBoundingBox bb(CBoundingBox::InvalidBB);

	for (INDEX_TYPE face_idx : tris)
	{
		bb.expand(mesh->vertex(face_idx, 0));
		bb.expand(mesh->vertex(face_idx, 1));
		bb.expand(mesh->vertex(face_idx, 2));
	}

	return bb;
}

KDNode2* KDNode2::build(CMesh* mesh)
{
	std::vector<INDEX_TYPE> tris;
	for (int i = 0; i < mesh->faces().size(); i++) tris.push_back(i);

	return build2( mesh, tris, 0 );
}


KDNode2* KDNode2::build2(CMesh* mesh, vector<INDEX_TYPE>& tris, int depth)
{
	KDNode2* node = new KDNode2();

	if (tris.empty()) return node;

	node->m_bbox = createBB(mesh, tris);

	if (tris.size() > 256)
	{
		int axis = node->m_bbox.longest_axis();

		double sum = 0.0;
		for (INDEX_TYPE idx : tris)
		{
			sum += createBB(mesh, idx).getMidpoint()[axis];
		}

		double midpt = sum / tris.size();

		vector<INDEX_TYPE> left_tris;
		vector<INDEX_TYPE> right_tris;

		for (INDEX_TYPE currentIndex : tris)
		{
			CBoundingBox tmpBB = createBB(mesh, currentIndex);

			if (tmpBB.getMin()[axis] >= midpt)
			{
				right_tris.push_back(currentIndex);
			}
			else
				if (tmpBB.getMax()[axis] <= midpt)
				{
					left_tris.push_back(currentIndex);
				}
				else
				{
					node->m_tris.push_back(currentIndex);
				}
		}

		node->m_left = KDNode2::build(mesh, left_tris, depth + 1);
		node->m_right = KDNode2::build(mesh, right_tris, depth + 1);
	}
	else
	{
		node->m_tris = tris;
		node->m_left = new KDNode2();
		node->m_right = new KDNode2();
	}

	return node;
}

KDNode2 * KDNode2::build(CMesh* mesh, vector<INDEX_TYPE> &tris, int depth)
{
	KDNode2 *node = new KDNode2();

	if ( tris.empty() ) return node;

	node->m_bbox = createBB(mesh, tris);

	if (tris.size() > 256)
	{
		CPoint3d midpt = node->m_bbox.getMidpoint();

		vector<INDEX_TYPE> left_tris;
		vector<INDEX_TYPE> right_tris;

		int axis = node->m_bbox.longest_axis();

		for (INDEX_TYPE currentIndex : tris)
		{
			CBoundingBox tmpBB = createBB(mesh, currentIndex);

			if (tmpBB.getMin()[axis] >= midpt[axis])
			{
				right_tris.push_back(currentIndex);
			}
			else if (tmpBB.getMax()[axis] <= midpt[axis])
			{
				left_tris.push_back(currentIndex);
			}
			else
			{
				node->m_tris.push_back(currentIndex);
			}
		}

		node->m_left = KDNode2::build(mesh, left_tris, depth + 1);
		node->m_right = KDNode2::build(mesh, right_tris, depth + 1);
	}
	else
	{
		node->m_tris = tris;
		node->m_left = new KDNode2();
		node->m_right = new KDNode2();
	}

	return node;
}


#include "Triangle.h"

bool KDNode2::hit(CMesh* mesh, const CPoint3d origin, const CVector3d dir, ShadeRec &sr)
{
	CPoint3d tmp;
	if ( m_bbox.hit(origin, dir, tmp) )
	{
		bool hit_tri = false;

		for ( INDEX_TYPE i : m_tris )
		{
			CPoint3d iP;
			CTriangle t(i, *mesh);
			CFace f = mesh->faces()[i];

			if ( t.hit(origin, dir, iP) )
			{
				if ((sr.vidx != f.A()) && (sr.vidx != f.B()) && (sr.vidx != f.C()))
				{
					double dot = dir.dotProduct(CVector3d(origin, iP));
					if (dot > 0)
					{
						hit_tri = true;
						sr.fidxs[ i ] = std::pair<double, CPoint3d>(dot, iP);
					}
				}
			}
		}

		bool ret = hit_tri && !sr.fidxs.empty();

		bool retLeft = m_left->hit(mesh, origin, dir, sr);
		bool retRight = m_right->hit(mesh, origin, dir, sr);

		return ret || retLeft || retRight;
	}
	return false;
}

bool KDNode2::hit(CMesh* mesh, const CPoint3d origin, const CVector3d dir, HitMap& hits, long vidx)
{
	CPoint3d tmp;
	if (m_bbox.hit(origin, dir, tmp))
	{
		bool hit_tri = false;

		for (INDEX_TYPE i : m_tris)
		{
			CPoint3d iP;
			CTriangle t(i, *mesh);
			CFace f = mesh->faces()[i];

			if (t.hit(origin, dir, iP))
			{
				if ((vidx != f.A()) && (vidx != f.B()) && (vidx != f.C()))
				{
					double dot = dir.dotProduct(CVector3d(origin, iP));
					if (dot > 0)
					{
						hit_tri = true;
						hits[i] = std::pair<double, CPoint3d>(dot, iP);
					}
				}
			}
		}

		bool ret = hit_tri && !hits.empty();

		bool retLeft = m_left->hit(mesh, origin, dir, hits, vidx);
		bool retRight = m_right->hit(mesh, origin, dir, hits, vidx);

		return ret || retLeft || retRight;
	}
	return false;
}

bool KDNode2::findCrossedBB(CBoundingBox bb, CMesh* mesh, std::set<INDEX_TYPE> &result)
{
	CBoundingBox ibb = CBoundingBox::intersection(bb, this->m_bbox);
	
	if ( bb.intersects( this->m_bbox ) )
	{
		bool lb = this->m_left->findCrossedBB(bb, mesh, result);

		bool rb = this->m_right->findCrossedBB(bb, mesh, result);

		for (INDEX_TYPE i : this->m_tris)
		{
			if ( bb.intersects( CTriangle(i, *mesh).getBoundingBox() ) )
				result.insert(i);
		}
	}

	return !result.empty();
}

