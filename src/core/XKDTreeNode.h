#pragma once
#ifndef XKDTREENODE_H
#define XKDTREENODE_H

#include <vector>

#include "XKDTreeBuffer.h"
#include "XVector.h"

#define MAXNODEDATASIZE 64

template<class _Typ, int _Dim> class XKDTreeNode
{
public:
	struct XKDTreeNodeBBox {
		XVector<_Typ, _Dim> Min;
		XVector<_Typ, _Dim> Max;
		XVector<_Typ, _Dim> Ctr;
		_Typ R;
	} m_bb;

	int splitaxis;

	XKDTreeNode* m_left;
	XKDTreeNode* m_right;

	XKDTreeBuffer<_Typ, _Dim>* m_kdtreeBuffer;

	std::vector<INDEX_TYPE> m_indices;

	XKDTreeNode(XKDTreeBuffer<_Typ, _Dim>* kdtreeBuffer, XKDTreeNode* parent = nullptr) : m_left(nullptr), m_right(nullptr), m_kdtreeBuffer(kdtreeBuffer)
	{
		if (parent != nullptr)
		{
			m_indices.reserve(parent->m_indices.size());
			m_bb.Min = m_bb.Max = kdtreeBuffer->dataPtr[parent->m_indices[0]];
		}
		else
		{
			m_indices.reserve(kdtreeBuffer->dataSize);
			m_bb.Min = m_bb.Max = kdtreeBuffer->dataPtr[0];
		}

	};

	~XKDTreeNode()
	{
		if (m_left != nullptr)
			delete m_left;

		if (m_right != nullptr)
			delete m_right;

		//m_indices.clear();
	}

	void insert(int idx, XVector<_Typ, _Dim>& e)
	{
		//XVector<_Typ, _Dim>& e = m_kdtreeBuffer->dataPtr[idx];

		for (int i = 0; i < _Dim; i++)
		{
			if (m_bb.Min[i] > e[i]) m_bb.Min[i] = e[i];
			if (m_bb.Max[i] < e[i]) m_bb.Max[i] = e[i];
		}

		m_indices.push_back(idx);
	}

	void build()//int level=0)
	{
		if (m_indices.size() <= MAXNODEDATASIZE)
		{
			return;
		}

		m_bb.Ctr = (m_bb.Max + m_bb.Min) / 2;
		m_bb.R = m_bb.Ctr.distTo(m_bb.Max);

		//szukam indeksu najd³u¿szej osi
		splitaxis = 0;
		_Typ max_diff = m_bb.Max[0] - m_bb.Min[0];
		for (int i = 1; i < _Dim; i++)
		{
			_Typ diff = m_bb.Max[i] - m_bb.Min[i];
			if (diff > max_diff)
			{
				max_diff = diff;
				splitaxis = i;
			}
		}

		_Typ right_min = m_bb.Min[splitaxis] + (max_diff / 2);

		m_left = new XKDTreeNode(m_kdtreeBuffer, this);
		m_right = new XKDTreeNode(m_kdtreeBuffer, this);

		for (auto idx : m_indices)
		{
			XVector<_Typ, _Dim>& e = m_kdtreeBuffer->dataPtr[idx];

			if (e[splitaxis] < right_min)
				m_left->insert(idx, e);
			else
				m_right->insert(idx, e);
		}

		m_indices.clear();
		std::vector<INDEX_TYPE>().swap(m_indices);

		if (m_left->m_indices.empty())
		{
			delete m_left;
			m_left = nullptr;
		}
		else
			m_left->build();// level + 1);

		if (m_right->m_indices.empty())
		{
			delete m_right;
			m_right = nullptr;
		}
		else
			m_right->build();// level + 1);
	}


	void find_k_closest_to_pt()
	{
		if (!m_indices.empty())
		{
			_Typ dist2;
			for (const int& idx : m_indices)
			{
				dist2 = m_kdtreeBuffer->pattern.dist2to(m_kdtreeBuffer->dataPtr[idx]);

				if ((dist2 < m_kdtreeBuffer->lastGood_d2) || (m_kdtreeBuffer->results.size() < m_kdtreeBuffer->searchLimit))
				{
					m_kdtreeBuffer->results.push_back(std::make_pair(dist2, idx));
					push_heap(m_kdtreeBuffer->results.begin(), m_kdtreeBuffer->results.end());
					// po tym najwiêkszy element trafia do results[0]

					if (m_kdtreeBuffer->results.size() > m_kdtreeBuffer->searchLimit)
					{
						pop_heap(m_kdtreeBuffer->results.begin(), m_kdtreeBuffer->results.end());
						// po tym najwiêkszy element trafia do results[k-1], a w results[0] jest drugi najwiêkszy
						
						m_kdtreeBuffer->results.pop_back();
						// usuwa results[k-1]
					}

					m_kdtreeBuffer->lastGood_d2 = m_kdtreeBuffer->results[0].first;
				}
			}
			m_kdtreeBuffer->lastGood_d = sqrt(m_kdtreeBuffer->lastGood_d2);
			return;
		}

		if ((m_kdtreeBuffer->results.size() >= m_kdtreeBuffer->searchLimit) && 
			(m_bb.Ctr.dist2to(m_kdtreeBuffer->pattern) >= sqr(m_bb.R + m_kdtreeBuffer->lastGood_d))	)
			return;

		_Typ myd = m_bb.Ctr[splitaxis] - m_kdtreeBuffer->pattern[splitaxis];

		if (myd >= 0.0f)
		{
			if (m_left != nullptr) m_left->find_k_closest_to_pt();
			if ((m_right != nullptr) && ((myd < m_kdtreeBuffer->lastGood_d) || (m_kdtreeBuffer->results.size() != m_kdtreeBuffer->searchLimit)))
				m_right->find_k_closest_to_pt();
		}
		else
		{
			if (m_right != nullptr) m_right->find_k_closest_to_pt();
			if ((m_left != nullptr) && ((-myd < m_kdtreeBuffer->lastGood_d) || (m_kdtreeBuffer->results.size() != m_kdtreeBuffer->searchLimit)))
				m_left->find_k_closest_to_pt();
		}
	}


	void find_all_in_distance_to_pt()
	{
		if (!m_indices.empty())
		{
			_Typ dist2;
			for (const int& idx : m_indices)
			{
				dist2 = m_kdtreeBuffer->pattern.dist2to(m_kdtreeBuffer->dataPtr[idx]);

				if (dist2 < m_kdtreeBuffer->maxdist2)
				{
					m_kdtreeBuffer->results.push_back(std::make_pair(dist2, idx));
				}
			}
			return;
		}

		if (m_bb.Ctr.dist2to(m_kdtreeBuffer->pattern) > sqr(m_bb.R + m_kdtreeBuffer->maxdist))
			return;

		_Typ myd = m_bb.Ctr[splitaxis] - m_kdtreeBuffer->pattern[splitaxis];

		if (myd >= 0.0f)
		{
			if (m_left != nullptr) m_left->find_all_in_distance_to_pt();
			if ((m_right != nullptr) && (myd <= m_kdtreeBuffer->maxdist)) m_right->find_all_in_distance_to_pt();
		}
		else
		{
			if (m_right != nullptr) m_right->find_all_in_distance_to_pt();
			if ((m_left != nullptr) && (-myd <= m_kdtreeBuffer->maxdist) ) m_left->find_all_in_distance_to_pt();
		}
	}


	void find_k_closest_to_ray()
	{
		if (!m_indices.empty())
		{
			_Typ dist2;
			for (const int& idx : m_indices)
			{
				dist2 = m_kdtreeBuffer->dataPtr[idx].dist2toRay(m_kdtreeBuffer->pattern, m_kdtreeBuffer->rayDir);

				if ((dist2 < m_kdtreeBuffer->lastGood_d2) || (m_kdtreeBuffer->results.size() < m_kdtreeBuffer->searchLimit))
				{
					m_kdtreeBuffer->results.push_back(std::make_pair(dist2, idx));
					push_heap(m_kdtreeBuffer->results.begin(), m_kdtreeBuffer->results.end());
					// po tym najwiêkszy element trafia do results[0]

					if (m_kdtreeBuffer->results.size() > m_kdtreeBuffer->searchLimit)
					{
						pop_heap(m_kdtreeBuffer->results.begin(), m_kdtreeBuffer->results.end());
						// po tym najwiêkszy element trafia do results[k-1], a w results[0] jest drugi najwiêkszy

						m_kdtreeBuffer->results.pop_back();
						// usuwa results[k-1]
					}

					m_kdtreeBuffer->lastGood_d2 = m_kdtreeBuffer->results[0].first;
				}
			}
			m_kdtreeBuffer->lastGood_d = sqrt(m_kdtreeBuffer->lastGood_d2);
			return;
		}

		// Check whether to abort
		if ((m_kdtreeBuffer->results.size() >= m_kdtreeBuffer->searchLimit) &&
			(m_bb.Ctr.dist2toRay(m_kdtreeBuffer->pattern, m_kdtreeBuffer->rayDir) >= sqr(m_bb.R + m_kdtreeBuffer->lastGood_d)))
			return;

		if (m_kdtreeBuffer->pattern[splitaxis] < m_bb.Ctr[splitaxis])
		{
			if (m_left != nullptr) m_left->find_k_closest_to_ray();
			if (m_right != nullptr) m_right->find_k_closest_to_ray();
		}
		else
		{
			if (m_right != nullptr) m_right->find_k_closest_to_ray();
			if (m_left != nullptr) m_left->find_k_closest_to_ray();
		}
	}


	void find_all_in_distance_to_ray()
	{
		if (!m_indices.empty())
		{
			_Typ dist2;
			for (const int& idx : m_indices)
			{
				dist2 = m_kdtreeBuffer->dataPtr[idx].dist2toRay(m_kdtreeBuffer->pattern, m_kdtreeBuffer->rayDir);

				if (dist2 < m_kdtreeBuffer->maxdist2)
				{
					m_kdtreeBuffer->results.push_back(std::make_pair(dist2, idx));
				}
			}
			return;
		}

		if (m_bb.Ctr.dist2toRay(m_kdtreeBuffer->pattern, m_kdtreeBuffer->rayDir) >= sqr(m_bb.R + m_kdtreeBuffer->maxdist))
			return;

		if (m_kdtreeBuffer->pattern[splitaxis] < m_bb.Ctr[splitaxis])
		{
			if (m_left != nullptr) m_left->find_all_in_distance_to_ray();
			if (m_right != nullptr) m_right->find_all_in_distance_to_ray();
		}
		else
		{
			if (m_right != nullptr) m_right->find_all_in_distance_to_ray();
			if (m_left != nullptr) m_left->find_all_in_distance_to_ray();
		}
	}

};

#endif