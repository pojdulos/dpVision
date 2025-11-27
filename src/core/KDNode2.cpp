#include "KDNode2.h"
#include "Mesh.h"
#include "dpLog.h"
#include <omp.h>

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

	#pragma omp parallel
	{
		CBoundingBox local_bb(CBoundingBox::InvalidBB);
		
		#pragma omp for nowait
		for (int i = 0; i < tris.size(); i++)
		{
			INDEX_TYPE face_idx = tris[i];
			local_bb.expand(mesh->vertex(face_idx, 0));
			local_bb.expand(mesh->vertex(face_idx, 1));
			local_bb.expand(mesh->vertex(face_idx, 2));
		}
		
		#pragma omp critical
		{
			bb.expand(local_bb);
		}
	}

	return bb;
}

KDNode2* KDNode2::build(CMesh* mesh, int maxTrisSize)
{
	std::vector<INDEX_TYPE> tris;
	for (int i = 0; i < mesh->faces().size(); i++)
		tris.push_back(i);

	return build( mesh, tris, 0, maxTrisSize);
}


KDNode2 * KDNode2::build(CMesh* mesh, std::vector<INDEX_TYPE> &tris, int depth, int maxTrisSize)
{
	KDNode2 *node = new KDNode2();

	if ( tris.empty() ) return node;

	// Ochrona przed zbyt du¿¹ g³êbokoœci¹
	const int MAX_DEPTH = 50;
	if (depth >= MAX_DEPTH)
	{
		dpWarn() << "Maximum depth reached at depth " << depth << " with " << tris.size() << " triangles.";
		node->m_tris = tris;
		node->m_bbox = createBB(mesh, tris);
		node->m_left = nullptr;
		node->m_right = nullptr;
		return node;
	}

	dpDebug() << "Building KDNode2 at depth " << depth << " with " << tris.size() << " triangles.";

	node->m_bbox = createBB(mesh, tris);

	if (tris.size() > maxTrisSize)
	{
		int axis = node->m_bbox.longest_axis();

		// Zbierz pary: (œrodek, indeks)
		std::vector<std::pair<double, INDEX_TYPE>> centerPairs;
		centerPairs.reserve(tris.size());

		#pragma omp parallel
		{
			int num_threads = omp_get_num_threads();
			int thread_id = omp_get_thread_num();

			std::vector<std::pair<double, INDEX_TYPE>> local_pairs;
			local_pairs.reserve(tris.size() / num_threads + 1);

			#pragma omp for nowait
			for (int i = 0; i < tris.size(); i++)
			{
				INDEX_TYPE idx = tris[i];
				double center = createBB(mesh, idx).getMidpoint()[axis];
				local_pairs.push_back(std::make_pair(center, idx));
			}

			#pragma omp critical
			{
				centerPairs.insert(centerPairs.end(), local_pairs.begin(), local_pairs.end());
			}
		}


		// Posortuj wed³ug œrodka
		std::sort(centerPairs.begin(), centerPairs.end(), 
			[](const std::pair<double, INDEX_TYPE>& a, const std::pair<double, INDEX_TYPE>& b) {
				return a.first < b.first;
			});

		// Podziel dok³adnie po po³owie (gwarantowany balans)
		size_t mid = centerPairs.size() / 2;

		std::vector<INDEX_TYPE> left_tris;
		std::vector<INDEX_TYPE> right_tris;
		left_tris.reserve(mid);
		right_tris.reserve(centerPairs.size() - mid);

		for (size_t i = 0; i < mid; i++)
		{
			left_tris.push_back(centerPairs[i].second);
		}

		for (size_t i = mid; i < centerPairs.size(); i++)
		{
			right_tris.push_back(centerPairs[i].second);
		}

		// Teraz left_tris i right_tris s¹ zawsze zbalansowane
		node->m_left = KDNode2::build(mesh, left_tris, depth + 1, maxTrisSize);
		node->m_right = KDNode2::build(mesh, right_tris, depth + 1, maxTrisSize);
	}
	else
	{
		node->m_tris = tris;
		node->m_left = nullptr;
		node->m_right = nullptr;
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

		// SprawdŸ potomków tylko jeœli nie s¹ pustymi wêz³ami
		bool retLeft = false;
		bool retRight = false;
		
		if (m_left)
		{
			retLeft = m_left->hit(mesh, origin, dir, sr);
		}
		
		if (m_right)
		{
			retRight = m_right->hit(mesh, origin, dir, sr);
		}

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

		// SprawdŸ potomków tylko jeœli nie s¹ pustymi wêz³ami
		bool retLeft = false;
		bool retRight = false;
		
		if (m_left)
		{
			retLeft = m_left->hit(mesh, origin, dir, hits, vidx);
		}
		
		if (m_right)
		{
			retRight = m_right->hit(mesh, origin, dir, hits, vidx);
		}

		return ret || retLeft || retRight;
	}
	return false;
}

bool KDNode2::findCrossedBB(CBoundingBox bb, CMesh* mesh, std::set<INDEX_TYPE> &result)
{
	if ( bb.intersects( this->m_bbox ) )
	{
		// SprawdŸ potomków tylko jeœli nie s¹ pustymi wêz³ami
		if (m_left)
		{
			m_left->findCrossedBB(bb, mesh, result);
		}

		if (m_right)
		{
			m_right->findCrossedBB(bb, mesh, result);
		}

		for (INDEX_TYPE i : this->m_tris)
		{
			if ( bb.intersects( CTriangle(i, *mesh).getBoundingBox() ) )
				result.insert(i);
		}
	}

	return !result.empty();
}

