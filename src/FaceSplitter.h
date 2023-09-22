#include <map>
#include <cstddef>

class CMesh;
class CVertex;

class __declspec(dllexport) CFaceSplitter
{
	CMesh *m;

	typedef std::pair<double, size_t> DI;
	typedef std::pair<size_t, size_t> EDGE;


	bool splitOneFace(size_t i1, EDGE &edge);
	double longestEdge(size_t fidx, EDGE &e);

public:
	std::map<DI, EDGE> bigFaces2;
	std::map<CVertex, size_t> newVertices;

	CFaceSplitter(CMesh *m1) :m(m1) {};

	void splitFaces(double EPS = 1.0);
};
