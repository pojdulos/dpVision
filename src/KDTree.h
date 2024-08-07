#ifndef KDTREE_H
#define KDTREE_H
#include "dll_global.h"

/*
Szymon Rusinkiewicz
Princeton University

KDtree.h
A K-D tree for points, with limited capabilities (find nearest point to
a given point, or to a ray).

Note that in order to be generic, this *doesn't* use Vecs and the like...
*/

#include <cstddef>
#include <vector>
#include <set>
#include <algorithm>

template< class RandomIt >
void sortHeap(RandomIt first, RandomIt last)
{
	while (first != last)
		std::pop_heap(first, last--);
}

template< class RandomIt, class Compare >
void sortHeap(RandomIt first, RandomIt last, Compare comp)
{
	while (first != last)
		std::pop_heap(first, last--, comp);
}

namespace trimesh {

using ::std::size_t;

// A point together with a distance - default comparison is by "first",
// i.e., distance
typedef std::pair<float, const float *> pt_with_d;

class DPVISION_EXPORT KDtree {
protected:
	struct Node;
	struct NodeStorageBlock;

	Node *root;
	NodeStorageBlock *storage;

	std::vector<NodeStorageBlock*> kontener;

	void build(const float *ptlist, size_t n);
	void build(const float **pts, size_t n);


public:
	// Compatibility function for closest-compatible-point searches
	struct CompatFunc
	{
		virtual bool operator () (const float *p) const = 0;
		virtual ~CompatFunc() {}  // To make the compiler shut up
	};

	// Constructors from an array or vector of points
	KDtree(const float *ptlist, size_t n) : root(NULL), storage(NULL)
		{ build(ptlist, n); }

	template <class T> KDtree(const ::std::vector<T> &v) : root(NULL), storage(NULL)
		{ build((const float *) &v[0], v.size()); }

	// Constructors from an array or vector of pointers to points
	KDtree(const float **pts, size_t n) : root(NULL), storage(NULL)
		{ build(pts, n); }

	template <class T> KDtree(::std::vector<T *> &pts) : root(NULL), storage(NULL)
		{ build((const float **) &pts[0], pts.size()); }

	// Destructor - frees the whole tree
	~KDtree();

	//=====================================================================

	// The queries: returns closest point to a point or a ray,
	// provided it's within sqrt(maxdist2) and is compatible
	const float *closest_to_pt(const float *p,
	                           float maxdist2 = 0.0f,
	                           const CompatFunc *iscompat = NULL) const;
	const float *closest_to_ray(const float *p, const float *dir,
	                            float maxdist2 = 0.0f,
	                            const CompatFunc *iscompat = NULL) const;

	//=====================================================================

	// Find the k nearest neighbors
	void find_k_closest_to_pt(::std::vector<const float *> &knn,
	                          int k,
	                          const float *p,
	                          float maxdist2 = 0.0f,
	                          const CompatFunc *iscompat = NULL) const;

	void find_k_closest_to_ray(::std::vector<const float *> &knn, 
								int k,
								const float *p, const float *dir,
								float maxdist2 = 0.0f,
								const CompatFunc *iscompat = NULL) const;

	//=====================================================================

	bool is_any_in_distance_to_pt(float distance, const float* p) const;

	void find_all_in_distance_to_pt(::std::vector<const float*>& knn,
		float distance,
		const float * p,
		const CompatFunc * iscompat = NULL) const;

	void find_all_in_distance_to_ray(::std::vector<const float*>& knn,
		float distance,
		const float * p, const float * dir,
		const CompatFunc * iscompat = NULL) const;

	void find_all_in_distance_to_ray_2(::std::vector<pt_with_d>& knn,
		float distance,
		const float * p, const float * dir,
		const CompatFunc * iscompat = NULL) const;
	//=====================================================================


};

} // namespace trimesh

#endif
