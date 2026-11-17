#pragma once
#include "Global.h"
#include "dll_global.h"
#include "Edge.h"

class CMesh;

// **ULEPSZONA WERSJA EdgeHasher - uwzglednia kanoniczna forme**
struct EdgeHasher {
    std::size_t operator()(const CEdge& e) const {
        // Normalize edge orientation before hashing.
        INDEX_TYPE a = std::min(e.first, e.second);
        INDEX_TYPE b = std::max(e.first, e.second);

        // Hash the canonicalized vertex pair.
        std::size_t h1 = std::hash<INDEX_TYPE>{}(a);
        std::size_t h2 = std::hash<INDEX_TYPE>{}(b);
        return h1 ^ (h2 << 1);
    }
};

class DPVISION_EXPORT  MeshTools
{
public:
	static void removeInvalidFaces(CMesh& mesh);
	static void removeDegenerateFaces(CMesh& mesh, float areaEps = 1e-10f);
	static void weldDuplicateVertices(CMesh& mesh, double eps=0.0);
	static void buildEdgeToFaces(const CMesh& mesh, std::unordered_map<CEdge, std::vector<INDEX_TYPE>, EdgeHasher>& edgeToFaces);
	static void fixNonManifoldEdges(CMesh& mesh);
	static void buildVertexToFaces(const CMesh& mesh, std::vector<std::vector<INDEX_TYPE>>& vertexFaces);
	static void fixNonManifoldVertices(CMesh& mesh);
	static void removeIsolatedVertices(CMesh& mesh);
	static void subdivideNarrowFaces(std::shared_ptr<CMesh> mesh, float aspectRatioThreshold, int maxIterations);
	static void repairMesh(CMesh& mesh);
};

