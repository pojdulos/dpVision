#ifndef XKDTREE_H
#define XKDTREE_H

#include "XKDTreeNode.h"
#include "XKDTreeBuffer.h"
#include "XVector.h"
#include <algorithm>


template<
	typename _Typ, int _Dim>
	class XKDTree
{
public:
	typedef XVector<_Typ, _Dim> Element;
	typedef XKDTreeNode<_Typ, _Dim> Node;

	Node* root;

	XKDTreeBuffer<_Typ, _Dim> buffer;

	XKDTree()
	{
		static_assert(std::is_arithmetic<_Typ>::value, "XKDTree data type must be numeric");
	}

	XKDTree(_Typ* data, int size)
	{
		static_assert(std::is_arithmetic<_Typ>::value, "XKDTree data type must be numeric");
		build(data, size);
	}

	~XKDTree()
	{
		if (root != nullptr)
			delete root;
	}

	//inline XKDTreeResult& getResults() { return buffer.results; }

	void build(_Typ* data, int size)
	{
		buffer.dataPtr = (Element*) data;
		buffer.dataSize = size;

		root = new Node(&buffer);

		for (int i = 0; i < size; i++)
		{
			root->insert(i, buffer.dataPtr[i]);
		}
		
		root->build();
	}

	size_t find_closest_to_pt(Element e, _Typ dist = -1.0f)
	{
		buffer.lastGood_d = buffer.lastGood_d2 = MAX_FLOAT;

		//buffer.maxdist = dist;
		buffer.maxdist2 = dist * dist;

		buffer.pattern = e;
		buffer.searchLimit = 1;

		buffer.results.clear();
		buffer.results.reserve(2);

		root->find_k_closest_to_pt();

		if (buffer.results[0].first > buffer.maxdist2)
		{
			buffer.results.clear();
			return 0;
		}

		return 1;
	}

	size_t find_closest_to_ray(Element e, Element dir, _Typ dist = -1.0f)
	{
		buffer.lastGood_d = buffer.lastGood_d2 = MAX_FLOAT;

		//buffer.maxdist = dist;
		buffer.maxdist2 = dist * dist;

		buffer.pattern = e;
		buffer.rayDir = dir;
		buffer.searchLimit = 1;

		buffer.results.clear();
		buffer.results.reserve(2);

		root->find_k_closest_to_ray();

		if (buffer.results[0].first > buffer.maxdist2)
		{
			buffer.results.clear();
			return 0;
		}

		return 1;
	}

	size_t find_k_closest_to_pt(int k, Element pt, _Typ dist = -1.0f)
	{
		buffer.lastGood_d = buffer.lastGood_d2 = MAX_FLOAT;

		//buffer.maxdist = dist;
		buffer.maxdist2 = dist * dist;

		buffer.pattern = pt;
		buffer.searchLimit = k;

		buffer.results.clear();
		buffer.results.reserve(k + 1);

		root->find_k_closest_to_pt();

		//std::make_heap(buffer.results.begin(), buffer.results.end());
		sortHeapX(buffer.results.begin(), buffer.results.end());

		if (dist != -1.0f)
			buffer.results.erase(
				std::remove_if(
					buffer.results.begin(),
					buffer.results.end(),
					[&](const std::pair<_Typ, int> k) { return (k.first > buffer.maxdist2); }),
				buffer.results.end());

		return buffer.results.size();
	}

	size_t find_k_closest_to_ray(int k, Element pt, Element dir, _Typ dist = -1.0)
	{
		buffer.lastGood_d = buffer.lastGood_d2 = MAX_FLOAT;

		//buffer.maxdist = dist;
		buffer.maxdist2 = dist * dist;

		buffer.pattern = pt;
		buffer.rayDir = dir;
		buffer.searchLimit = k;

		buffer.results.clear();
		buffer.results.reserve(k + 1);

		root->find_k_closest_to_ray();

		sortHeapX(buffer.results.begin(), buffer.results.end());

		if (dist != -1.0f)
			buffer.results.erase(
				std::remove_if(
					buffer.results.begin(),
					buffer.results.end(),
					[&](const std::pair<_Typ, int> k) { return (k.first > buffer.maxdist2); }),
				buffer.results.end());

		return buffer.results.size();
	}

	size_t find_all_in_distance_to_pt(Element pt, _Typ dist)
	{
		//buffer.lastGood_d = buffer.lastGood_d2 = MAX_FLOAT;

		buffer.maxdist = dist;
		buffer.maxdist2 = dist * dist;

		buffer.pattern = pt;
		//buffer.searchLimit = 0;

		buffer.results.clear();

		root->find_all_in_distance_to_pt();

		std::make_heap(buffer.results.begin(), buffer.results.end());
		sortHeapX(buffer.results.begin(), buffer.results.end());

		return buffer.results.size();
	}

	size_t find_all_in_distance_to_ray(Element pt, Element dir, _Typ dist)
	{
		//buffer.lastGood_d = buffer.lastGood_d2 = MAX_FLOAT;

		buffer.maxdist = dist;
		buffer.maxdist2 = dist * dist;

		buffer.pattern = pt;
		buffer.rayDir = dir;

		buffer.results.clear();

		root->find_all_in_distance_to_ray();

		std::make_heap(buffer.results.begin(), buffer.results.end());
		sortHeapX(buffer.results.begin(), buffer.results.end());

		return buffer.results.size();
	}
};

template<typename _Typ> class XKDTree3 : public XKDTree<_Typ, 3>
{
public:
	using XKDTree::XKDTree;
	using XKDTree::build;

	XKDTree3(CTriple<_Typ>* data, int size) : XKDTree<_Typ, 3>(data[0].v, size) {}
	XKDTree3(std::vector<CTriple<_Typ>>& v) : XKDTree<_Typ, 3>(v[0].v, v.size()) {}
	XKDTree3(std::vector<CPoint3<_Typ>>& v) : XKDTree<_Typ, 3>(v[0].v, v.size()) {}
	XKDTree3(std::vector<CVector3<_Typ>>& v) : XKDTree<_Typ, 3>(v[0].v, v.size()) {}

	void build(CTriple<_Typ>* data, int size) { XKDTree<_Typ, 3>::build(data[0].v, size); }
	void build(std::vector<CTriple<_Typ>>& v) { XKDTree<_Typ, 3>::build(v[0].v, v.size()); }
	void build(std::vector<CPoint3<_Typ>>& v) { XKDTree<_Typ, 3>::build(v[0].v, v.size()); }
	void build(std::vector<CVector3<_Typ>>& v) { XKDTree<_Typ, 3>::build(v[0].v, v.size()); }
};

class XKDTree3d : public XKDTree3<double>
{
public:
	using XKDTree3::XKDTree3;
	using XKDTree3::build;
};

#include "Vertex.h"

class XKDTree3f : public XKDTree3<float>
{
public:
	using XKDTree3::XKDTree3;
	using XKDTree3::build;

	XKDTree3f(std::vector<CVertex>& v) : XKDTree3<CVertex::Type>(&v[0], v.size()) {}

	void build(std::vector<CVertex>& v) { XKDTree3<CVertex::Type>::build(&v[0], v.size()); }
};


#endif
