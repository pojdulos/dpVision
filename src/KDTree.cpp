/*
Szymon Rusinkiewicz
Princeton University

KDtree.cc
A K-D tree for points, with limited capabilities (find nearest point to
a given point, or to a ray).
*/

#include "KDtree.h"

#include <cstddef>
#include <cstring>
#include <cmath>
#include <vector>
#include <utility>
#include <algorithm>
using namespace std;

#if defined(_MSC_VER)
#  define inline __forceinline
#elif defined(__GNUC__) && (__GNUC__ > 3)
#  define inline __inline __attribute__ ((__always_inline__))
#endif


namespace trimesh {

// Small utility fcns - including them keeps this file independent of Vec.h
static inline float sqr(float x)
{
	return x * x;
}

static inline float dist2(const float *x, const float *y)
{
	return sqr(x[0] - y[0]) + sqr(x[1] - y[1]) + sqr(x[2] - y[2]);
}

static inline float dist2ray2(const float *x, const float *p, const float *d)
{
	float xp0 = x[0] - p[0], xp1 = x[1] - p[1], xp2 = x[2] - p[2];
	return sqr(xp0) + sqr(xp1) + sqr(xp2) -
	       sqr(xp0 * d[0] + xp1 * d[1] + xp2 * d[2]);
}




// Class for nodes in the K-D tree
struct KDtree::Node {
	// A place to put all the stuff required while traversing the K-D
	// tree, so we don't have to pass tons of variables at each fcn call
	struct Traversal_Info {
		const float *p, *dir;
		const float *closest;
		float closest_d, closest_d2;
		const KDtree::CompatFunc *iscompat;
		size_t k;
		vector<pt_with_d> knn;
	};

	enum { MAX_PTS_PER_NODE = 64 };

	// The node itself

	int npts; // If this is 0, intermediate node.  If nonzero, leaf.

	union {
		struct {
			float center[3];
			float r;
			int splitaxis;
			Node *child1, *child2;
		} node;
		struct {
			const float *p[MAX_PTS_PER_NODE];
		} leaf;
	};

	static Node *alloc(KDtree *kd);
	void build(KDtree *kd, const float **pts, size_t n);
	void find_closest_to_pt(Traversal_Info &ti) const;
	void find_k_closest_to_pt(Traversal_Info &ti) const;
	void find_closest_to_ray(Traversal_Info &ti) const;
	void find_k_closest_to_ray(Traversal_Info &ti) const;
	void find_all_in_distance_to_pt(KDtree::Node::Traversal_Info &ti) const;
	void find_all_in_distance_to_ray(KDtree::Node::Traversal_Info &ti) const;
};


// Storage for Nodes in the KDtree.  We allocate large-ish blocks, each
// of which can store many Nodes.  When we delete the tree, we can free
// the blocks without having to crawl through all the Nodes.
struct KDtree::NodeStorageBlock {
	enum { NODES_PER_BLOCK = 100 };
	char storage[NODES_PER_BLOCK * sizeof(Node)];

	Node *next_avail;
	NodeStorageBlock *next_block;

	NodeStorageBlock(NodeStorageBlock *next_block_ = NULL) :
		next_avail((Node *) &storage[0]), next_block(next_block_)
		{}

	// Try to allocate from this block, returning storage for a new XKDTreeNode
	// or NULL if we can't.  In the latter case, it is the responsibility
	// of the caller (i.e., KDtree::XKDTreeNode::alloc) to create a new block.
	inline Node *alloc()
	{
		Node *start = (Node *) &storage[0];
		if (next_avail - start >= NODES_PER_BLOCK)
			return NULL;
		return next_avail++;
	}

	// Delete chain of blocks
	~NodeStorageBlock()
	{
	//	if (next_block)
	//		delete next_block;
	}
};


// Allocate a node in a NodeStorageBlock owned by the KDtree
inline KDtree::Node *KDtree::Node::alloc(KDtree *kd)
{
	Node *node = kd->storage->alloc();
	if (!node) {
		// Create a new NodeStorageBlock, and make kd->storage
		// point to it.  This block stores a pointer to the previous
		// head of the list, so that we can delete it later.
		kd->storage = new NodeStorageBlock(kd->storage);
		kd->kontener.push_back(kd->storage);
		node = kd->storage->alloc();
	}

	return node;
}


// Create a KD tree from the points pointed to by the array pts
void KDtree::Node::build(KDtree *kd, const float **pts, size_t n)
{
	// Leaf nodes
	if (n <= MAX_PTS_PER_NODE) {
		npts = n;
		memcpy(leaf.p, pts, n * sizeof(float *));
		return;
	}


	// Else, interior nodes
	npts = 0;

	// Find bbox
	float xmin = pts[0][0], xmax = pts[0][0];
	float ymin = pts[0][1], ymax = pts[0][1];
	float zmin = pts[0][2], zmax = pts[0][2];

	for (size_t i = 1; i < n; i++) {
		if      (pts[i][0] < xmin) xmin = pts[i][0];
		else if (pts[i][0] > xmax) xmax = pts[i][0];
		if      (pts[i][1] < ymin) ymin = pts[i][1];
		else if (pts[i][1] > ymax) ymax = pts[i][1];
		if      (pts[i][2] < zmin) zmin = pts[i][2];
		else if (pts[i][2] > zmax) zmax = pts[i][2];
	}

	// Find node center and size
	float rx = 0.5f * (xmax - xmin);
	float ry = 0.5f * (ymax - ymin);
	float rz = 0.5f * (zmax - zmin);
	node.center[0] = xmin + rx;
	node.center[1] = ymin + ry;
	node.center[2] = zmin + rz;
	node.r = sqrt(sqr(rx) + sqr(ry) + sqr(rz));

	// Find longest axis
	node.splitaxis = 2;
	if (rx > ry) {
		if (rx > rz)
			node.splitaxis = 0;
	} else {
		if (ry > rz)
			node.splitaxis = 1;
	}

	// Bentley-McIlroy 3-way partition
	const float splitval = node.center[node.splitaxis];
	const float **last = pts + n - 1;
	const float **less = pts, **greater = last;
	const float **left = pts, **right = last;
	while (1) {
		while (1) {
			float val = (*left)[node.splitaxis];
			if (val < splitval) {
				left++;
				if (left > right)
					goto crossed;
			} else if (val == splitval) {
				swap(*left++, *less++);
				if (left > right)
					goto crossed;
			} else {
				break;
			}
		}
		while (1) {
			float val = (*right)[node.splitaxis];
			if (val > splitval) {
				right--;
				if (left > right)
					goto crossed;
			} else if (val == splitval) {
				swap(*right--, *greater--);
				if (left > right)
					goto crossed;
			} else {
				break;
			}
		}
		swap(*left++, *right--);
		if (left > right)
			break;
	}

crossed:

	less--; left--;
	while (less >= pts)
		swap(*less--, *left--);

	greater++; right++;
	while (greater <= last)
		swap(*greater++, *right++);

	// left points to the last element < splitval, and
	// right points to the first element > splitval
	size_t num_less = left - pts + 1;
	size_t num_leq = right - pts;
	size_t ideal_split = n / 2;
	size_t split_left = (num_leq <= ideal_split) ? num_leq :
	                    (num_less >= ideal_split) ? num_less :
	                    ideal_split;

	// Build subtrees
	node.child1 = alloc(kd);
	node.child1->build(kd, pts, split_left);
	node.child2 = alloc(kd);
	node.child2->build(kd, pts + split_left, n - split_left);
}


// Crawl the KD tree
void KDtree::Node::find_closest_to_pt(KDtree::Node::Traversal_Info &ti) const
{
	// Leaf nodes
	if (npts) {
		for (int i = 0; i < npts; i++) {
			float myd2 = dist2(leaf.p[i], ti.p);
			if ((myd2 < ti.closest_d2) &&
			    leaf.p[i] != ti.p &&
			    (!ti.iscompat || (*ti.iscompat)(leaf.p[i]))) {
				ti.closest_d2 = myd2;
				ti.closest_d = sqrt(ti.closest_d2);
				ti.closest = leaf.p[i];
			}
		}
		return;
	}


	// Check whether to abort
	if (dist2(node.center, ti.p) >= sqr(node.r + ti.closest_d))
		return;

	// Recursive case - pick the optimal order
	float myd = node.center[node.splitaxis] - ti.p[node.splitaxis];
	if (myd >= 0.0f) {
		node.child1->find_closest_to_pt(ti);
		if (myd < ti.closest_d)
			node.child2->find_closest_to_pt(ti);
	} else {
		node.child2->find_closest_to_pt(ti);
		if (-myd < ti.closest_d)
			node.child1->find_closest_to_pt(ti);
	}
}


// Crawl the KD tree, retaining k closest points
void KDtree::Node::find_k_closest_to_pt(KDtree::Node::Traversal_Info &ti) const
{
	// Leaf nodes
	if (npts) {
		for (int i = 0; i < npts; i++)
		{
			float myd2 = dist2(leaf.p[i], ti.p);

			if ( ((myd2 < ti.closest_d2) || (ti.knn.size() < ti.k))
				// && leaf.p[i] != ti.p
				&& (!ti.iscompat || (*ti.iscompat)(leaf.p[i])))
			{
				float myd = sqrt(myd2);
				ti.knn.push_back(make_pair(myd, leaf.p[i]));
				push_heap(ti.knn.begin(), ti.knn.end());      // po tym najwiêkszy element trafia do knn[0]
				
				if (ti.knn.size() > ti.k)
				{
					pop_heap(ti.knn.begin(), ti.knn.end());  // po tym najwiêkszy element trafia do knn[k-1], a w knn[0] jest drugi najwiêkszy
					ti.knn.pop_back();
				}
				
				// Keep track of distance to k-th closest
				ti.closest_d = ti.knn[0].first;              // w knn[0] powinien byc najwiêkszy element
				ti.closest_d2 = sqr(ti.closest_d); 
			}
		}
		return;
	}


	// Check whether to abort
	if (dist2(node.center, ti.p) >= sqr(node.r + ti.closest_d) &&
	    ti.knn.size() == ti.k)
		return;

	// Recursive case - pick the optimal order
	float myd = node.center[node.splitaxis] - ti.p[node.splitaxis];
	if (myd >= 0.0f) {
		node.child1->find_k_closest_to_pt(ti);
		if (myd < ti.closest_d || ti.knn.size() != ti.k)
			node.child2->find_k_closest_to_pt(ti);
	} else {
		node.child2->find_k_closest_to_pt(ti);
		if (-myd < ti.closest_d || ti.knn.size() != ti.k)
			node.child1->find_k_closest_to_pt(ti);
	}
}


void KDtree::Node::find_all_in_distance_to_pt(KDtree::Node::Traversal_Info &ti) const
{
	// Leaf nodes
	if (npts) {
		for (int i = 0; i < npts; i++) {
			float myd2 = dist2(leaf.p[i], ti.p);
			if ((myd2 < ti.closest_d2) && (leaf.p[i] != ti.p) && (!ti.iscompat || (*ti.iscompat)(leaf.p[i])))
			{
				float myd = sqrt(myd2);
				ti.knn.push_back(make_pair(myd, leaf.p[i]));
			}
		}
		return;
	}


	// Check whether to abort
	if ( dist2(node.center, ti.p) >= sqr(node.r + ti.closest_d) )
		return;

	// Recursive case - pick the optimal order
	float myd = node.center[node.splitaxis] - ti.p[node.splitaxis];
	if (myd >= 0.0f) {
		node.child1->find_all_in_distance_to_pt(ti);
		if (myd < ti.closest_d)
			node.child2->find_all_in_distance_to_pt(ti);
	}
	else {
		node.child2->find_all_in_distance_to_pt(ti);
		if (-myd < ti.closest_d)
			node.child1->find_all_in_distance_to_pt(ti);
	}
}



// Crawl the KD tree to look for the closest point to
// the line going through ti.p in the direction ti.dir
void KDtree::Node::find_closest_to_ray(KDtree::Node::Traversal_Info &ti) const
{
	// Leaf nodes
	if (npts) {
		for (int i = 0; i < npts; i++) {
			// We don't want to find the point itself
			if (leaf.p[i] == ti.p)
				continue;
			float myd2 = dist2ray2(leaf.p[i], ti.p, ti.dir);
			if ((myd2 < ti.closest_d2) &&
			    (!ti.iscompat || (*ti.iscompat)(leaf.p[i]))) {
				ti.closest_d2 = myd2;
				ti.closest_d = sqrt(ti.closest_d2);
				ti.closest = leaf.p[i];
			}
		}
		return;
	}


	// Check whether to abort
	if (dist2ray2(node.center, ti.p, ti.dir) >= sqr(node.r + ti.closest_d))
		return;

	// Recursive case - pick the optimal order
	if (ti.p[node.splitaxis] < node.center[node.splitaxis] ) {
		node.child1->find_closest_to_ray(ti);
		node.child2->find_closest_to_ray(ti);
	} else {
		node.child2->find_closest_to_ray(ti);
		node.child1->find_closest_to_ray(ti);
	}
}


// Crawl the KD tree, retaining k closest points
void KDtree::Node::find_k_closest_to_ray(KDtree::Node::Traversal_Info &ti) const
{
	// Leaf nodes
	if (npts) {
		for (int i = 0; i < npts; i++) {
			// We don't want to find the point itself  <--- a tak w³aœciwie, to dlaczego ????
			if (leaf.p[i] == ti.p)
				continue;

			float myd2 = dist2ray2(leaf.p[i], ti.p, ti.dir); //kwadrat odleg³oœci punktu od promienia
			
			//if ((myd2 < ti.closest_d2 || ti.knn.size() < ti.k) && (!ti.iscompat || (*ti.iscompat)(leaf.p[i])))
			if ( ( myd2 < ti.closest_d2 ) && (!ti.iscompat || (*ti.iscompat)(leaf.p[i])))
			{
				float myd = sqrt(myd2);
				
				ti.knn.push_back(make_pair(myd, leaf.p[i])); //dok³ada element do wektora
				push_heap(ti.knn.begin(), ti.knn.end());	// przesuwa najwiêkszy element na pocz¹tek wektora
				
				if (ti.knn.size() > ti.k) {
					pop_heap(ti.knn.begin(), ti.knn.end()); // przesuwa najwiêkszy element na koniec wektora,
															// a jednoczeœnie na pocz¹tku pojawia sie drugi w kolejnoœci element
					ti.knn.pop_back();						// usuwa ostatni element wektora
				}

				// Keep track of distance to k-th closest
				ti.closest_d = ti.knn[0].first;				// teraz pierwszy element wektora zawiera najbardziej odleg³y punkt z wybranych
				ti.closest_d2 = sqr(ti.closest_d);
			}
		}
		return;
	}



	// Check whether to abort
	if (dist2ray2(node.center, ti.p, ti.dir) >= sqr(node.r + ti.closest_d) && (ti.knn.size() == ti.k) )
		return;

	// Recursive case - pick the optimal order
	if (ti.p[node.splitaxis] < node.center[node.splitaxis]) {
		node.child1->find_k_closest_to_ray(ti);
		node.child2->find_k_closest_to_ray(ti);
	}
	else {
		node.child2->find_k_closest_to_ray(ti);
		node.child1->find_k_closest_to_ray(ti);
	}
}



// Crawl the KD tree, retaining k closest points
void KDtree::Node::find_all_in_distance_to_ray(KDtree::Node::Traversal_Info &ti) const
{
	// Leaf nodes
	if (npts) {
		for (int i = 0; i < npts; i++) {

			float myd2 = dist2ray2(leaf.p[i], ti.p, ti.dir); //kwadrat odleg³oœci punktu od promienia

			if ((myd2 < ti.closest_d2) && (!ti.iscompat || (*ti.iscompat)(leaf.p[i])))
			{
				float myd = sqrt(myd2);

				ti.knn.push_back(make_pair(myd, leaf.p[i])); //dok³ada element do wektora
			}
		}
		return;
	}



	// Check whether to abort
	if ( dist2ray2(node.center, ti.p, ti.dir) >= sqr(node.r + ti.closest_d) )
		return;

	// Recursive case - pick the optimal order
	if (ti.p[node.splitaxis] < node.center[node.splitaxis]) {
		node.child1->find_all_in_distance_to_ray(ti);
		node.child2->find_all_in_distance_to_ray(ti);
	}
	else {
		node.child2->find_all_in_distance_to_ray(ti);
		node.child1->find_all_in_distance_to_ray(ti);
	}
}



// Create a KDtree from a list of points (i.e., ptlist is a list of 3*n floats)
void KDtree::build(const float *ptlist, size_t n)
{
	if (!n)
		return;

	vector<const float *> pts(n);
	for (size_t i = 0; i < n; i++)
		pts[i] = ptlist + i * 3;

	build(&pts[0], n);
}


// Create a KDtree from a list of pointers to points
void KDtree::build(const float **pts, size_t n)
{
	if (!n)
		return;

	storage = new NodeStorageBlock;
	kontener.push_back(storage);
	root = Node::alloc(this);
	root->build(this, pts, n);
}


// Delete a KDtree
KDtree::~KDtree()
{
	while (!kontener.empty())
	{
		delete kontener.back();
		kontener.pop_back();
	}
//	if (storage)
//		delete storage;
	storage = NULL;
	root = NULL;
}


// Return the closest point in the KD tree to p
const float *KDtree::closest_to_pt(const float *p, float maxdist2 /* = 0.0f */,
                                   const CompatFunc *iscompat /* = NULL */) const
{
	if (!root || !p)
		return NULL;

	Node::Traversal_Info ti;
	ti.p = p;
	ti.iscompat = iscompat;
	ti.closest = NULL;
	
	if (maxdist2 <= 0.0f)
		maxdist2 = sqr(root->node.r);
	
	ti.closest_d2 = maxdist2;
	ti.closest_d = sqrt(ti.closest_d2);

	root->find_closest_to_pt(ti);

	return ti.closest;
}


// Return the closest point in the KD tree to the line
// going through p in the direction dir
const float *KDtree::closest_to_ray(const float *p, const float *dir,
                                    float maxdist2 /* = 0.0f */,
                                    const CompatFunc *iscompat /* = NULL */) const
{
	if (!root || !p || !dir)
		return NULL;

	float one_over_dir_len = 1.0f / sqrt(sqr(dir[0])+sqr(dir[1])+sqr(dir[2]));
	float normalized_dir[3] = { dir[0] * one_over_dir_len,
	                            dir[1] * one_over_dir_len,
	                            dir[2] * one_over_dir_len };
	Node::Traversal_Info ti;
	ti.dir = normalized_dir;
	ti.p = p;
	ti.iscompat = iscompat;
	ti.closest = NULL;
	
	if (maxdist2 <= 0.0f)
		maxdist2 = sqr(root->node.r);
	
	ti.closest_d2 = maxdist2;
	ti.closest_d = sqrt(ti.closest_d2);

	root->find_closest_to_ray(ti);

	return ti.closest;
}


// Find the k nearest neighbors
void KDtree::find_k_closest_to_pt(std::vector<const float *> &knn,
                                  int k,
                                  const float *p,
                                  float maxdist2 /* = 0.0f */,
                                  const CompatFunc *iscompat /* = NULL */) const
{
	knn.clear();
	if (!root || !p)
		return;

	Node::Traversal_Info ti;
	ti.p = p;
	ti.iscompat = iscompat;
	ti.closest = NULL;
	
	if (maxdist2 <= 0.0f)
		maxdist2 = sqr(root->node.r);
	
	ti.closest_d2 = maxdist2;
	ti.closest_d = sqrt(ti.closest_d2);
	
	ti.k = k;
	ti.knn.reserve(k+1);

	root->find_k_closest_to_pt(ti);

	size_t found = ti.knn.size();
	if (!found)
		return;

	knn.resize(found);
	sortHeap(ti.knn.begin(), ti.knn.end());
	for (size_t i = 0; i < found; i++)
		knn[i] = ti.knn[i].second;
}

void KDtree::find_k_closest_to_ray(::std::vector<const float*>& knn, int k, const float * p, const float * dir, float maxdist2, const CompatFunc * iscompat) const
{
	knn.clear();
	if (!root || !p || !dir)
		return;

	float one_over_dir_len = 1.0f / sqrt(sqr(dir[0]) + sqr(dir[1]) + sqr(dir[2]));
	float normalized_dir[3] = { dir[0] * one_over_dir_len,
								dir[1] * one_over_dir_len,
								dir[2] * one_over_dir_len };
	Node::Traversal_Info ti;
	ti.dir = normalized_dir;
	ti.p = p;
	ti.iscompat = iscompat;
	ti.closest = NULL;
	
	if (maxdist2 <= 0.0f)
		maxdist2 = sqr(root->node.r);
	
	ti.closest_d2 = maxdist2;
	ti.closest_d = sqrt(ti.closest_d2);

	ti.k = k;
	ti.knn.reserve(k + 1);

	root->find_k_closest_to_ray(ti);

	size_t found = ti.knn.size();
	if (!found)
		return;

	knn.resize(found);
	sortHeap(ti.knn.begin(), ti.knn.end());
	for (size_t i = 0; i < found; i++)
		knn[i] = ti.knn[i].second;
}


void KDtree::find_all_in_distance_to_pt(::std::vector<const float*>& knn, float distance, const float * p, const CompatFunc * iscompat) const
{
	knn.clear();
	if (!root || !p)
		return;

	Node::Traversal_Info ti;
	ti.p = p;
	ti.iscompat = iscompat;
	ti.closest = NULL;
	
	ti.closest_d = distance;
	ti.closest_d2 = sqr(ti.closest_d);

	root->find_all_in_distance_to_pt(ti);

	size_t found = ti.knn.size();
	if (!found)
		return;

	knn.resize(found);
	sortHeap(ti.knn.begin(), ti.knn.end());
	for (size_t i = 0; i < found; i++)
		knn[i] = ti.knn[i].second;
}

void KDtree::find_all_in_distance_to_ray(::std::vector<const float*>& knn, float distance, const float * p, const float * dir, const CompatFunc * iscompat) const
{
	knn.clear();
	if (!root || !p || !dir)
		return;

	float one_over_dir_len = 1.0f / sqrt(sqr(dir[0]) + sqr(dir[1]) + sqr(dir[2]));
	float normalized_dir[3] = { dir[0] * one_over_dir_len,
								dir[1] * one_over_dir_len,
								dir[2] * one_over_dir_len };
	Node::Traversal_Info ti;
	ti.dir = normalized_dir;
	ti.p = p;
	ti.iscompat = iscompat;
	ti.closest = NULL;
	
	ti.closest_d = distance;
	ti.closest_d2 = sqr(ti.closest_d);

	root->find_all_in_distance_to_ray(ti);

	size_t found = ti.knn.size();
	if (!found)
		return;

	knn.resize(found);
	
	make_heap(ti.knn.begin(), ti.knn.end());
	sortHeap(ti.knn.begin(), ti.knn.end());
	
	for (size_t i = 0; i < found; i++)
		knn[i] = ti.knn[i].second;
}

void KDtree::find_all_in_distance_to_ray_2(::std::vector<pt_with_d>& knn, float distance, const float * p, const float * dir, const CompatFunc * iscompat) const
{
	if (!root || !p || !dir)
		return;

	float one_over_dir_len = 1.0f / sqrt(sqr(dir[0]) + sqr(dir[1]) + sqr(dir[2]));
	float normalized_dir[3] = { dir[0] * one_over_dir_len,
								dir[1] * one_over_dir_len,
								dir[2] * one_over_dir_len };
	Node::Traversal_Info ti;
	ti.dir = normalized_dir;
	ti.p = p;
	ti.iscompat = iscompat;
	ti.closest = NULL;
	
	ti.closest_d = distance;
	ti.closest_d2 = sqr(ti.closest_d);

	root->find_all_in_distance_to_ray(ti);

	size_t found = ti.knn.size();
	if (!found)
		return;

	make_heap(ti.knn.begin(), ti.knn.end());
	sortHeap(ti.knn.begin(), ti.knn.end());

	knn = ti.knn;
}


} // namespace trimesh
