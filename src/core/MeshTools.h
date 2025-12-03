#pragma once
#include "Global.h"
#include "Edge.h"

class CMesh;

// **ULEPSZONA WERSJA EdgeHasher - uwzglêdnia kanoniczn¹ formê**
struct EdgeHasher {
    std::size_t operator()(const CEdge& e) const {
        // **KLUCZOWE: Normalizuj kolejnoœæ przed hashowaniem**
        INDEX_TYPE a = std::min(e.first, e.second);
        INDEX_TYPE b = std::max(e.first, e.second);

        // FNV-1a hash z unormalizowanych wartoœci
        std::size_t h1 = std::hash<INDEX_TYPE>{}(a);
        std::size_t h2 = std::hash<INDEX_TYPE>{}(b);
        return h1 ^ (h2 << 1);
    }
};

// **OPCJONALNIE: Dodaj operator== dla CEdge jeœli go nie ma**
// To zapewni poprawne dzia³anie w unordered_map/set
namespace std {
    template<>
    struct equal_to<CEdge> {
        bool operator()(const CEdge& a, const CEdge& b) const {
            // Porównuj w kanonicznej formie
            INDEX_TYPE a1 = std::min(a.first, a.second);
            INDEX_TYPE a2 = std::max(a.first, a.second);
            INDEX_TYPE b1 = std::min(b.first, b.second);
            INDEX_TYPE b2 = std::max(b.first, b.second);
            return (a1 == b1) && (a2 == b2);
        }
    };
}

class MeshTools
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

