#include <algorithm>
#include <limits>
#include <cmath>
#include <array>

#include "Mesh.h"

struct Ray {
    CPoint3f origin;    // Pocz¹tek promienia
    CVector3f direction; // Kierunek promienia
};

// AABB - Axis-Aligned Bounding Box
struct AABB {
    CPoint3f min, max;

    // Rozszerz AABB o punkt
    void expand(const CPoint3f& p) {
        min = { std::min(min.x, p.x), std::min(min.y, p.y), std::min(min.z, p.z) };
        max = { std::max(max.x, p.x), std::max(max.y, p.y), std::max(max.z, p.z) };
    }

    // Sprawdzenie przeciêcia z innym AABB
    bool intersects(const AABB& other) const {
        return (min.x <= other.max.x && max.x >= other.min.x) &&
            (min.y <= other.max.y && max.y >= other.min.y) &&
            (min.z <= other.max.z && max.z >= other.min.z);
    }
};

// Wêze³ BVH
struct BVHNode {
    AABB bbox;
    int leftChild = -1;   // Indeks lewego dziecka
    int rightChild = -1;  // Indeks prawego dziecka
    std::vector<int> triangleIndices; // Indeksy trójk¹tów (dla liœci)
};

// Budowa BVH
class BVH {
public:
    BVH(CMesh *mesh) : triangles(mesh->faces()), vertices(mesh->vertices()) {
        rootNode = build(0, mesh->faces().size(), 0);
    }

    BVH(const CMesh::Faces& triangles, const CMesh::Vertices& vertices) : triangles(triangles), vertices(vertices) {
        rootNode = build(0, triangles.size(), 0);
    }

    const std::vector<BVHNode>& getNodes() const { return nodes; }


    // Funkcja do sprawdzenia przeciêcia promienia z trójk¹tem (algorytm Möllera-Trumbore)
    bool rayIntersectsTriangle(const Ray& ray,
        const CPoint3f& V0, const CPoint3f& V1, const CPoint3f& V2,
        float& t, float& u, float& v) {
        const float _EPSILON = 1e-6f; // Tolerancja numeryczna

        // Krawêdzie trójk¹ta
        CVector3f E1 = V1 - V0;
        CVector3f E2 = V2 - V0;

        // Wektor normalny do p³aszczyzny trójk¹ta
        CVector3f P = ray.direction.crossProduct(E2);

        // Wyznacznik macierzy (determinant)
        float det = E1.dotProduct(P);

        // Jeœli det jest bliski zeru, promieñ jest równoleg³y do p³aszczyzny trójk¹ta
        if (fabs(det) < _EPSILON) return false;

        float invDet = 1.0f / det;

        // Wektor od V0 do pocz¹tku promienia
        CVector3f T = ray.origin - V0;

        // Oblicz wspó³czynnik u (po³o¿enie barycentryczne)
        u = T.dotProduct(P) * invDet;
        if (u < 0.0f || u > 1.0f) return false;

        // Oblicz wspó³czynnik v
        CVector3f Q = T.crossProduct(E1);
        v = ray.direction.dotProduct(Q) * invDet;
        if (v < 0.0f || u + v > 1.0f) return false;

        // Oblicz t (odleg³oœæ od pocz¹tku promienia)
        t = E2.dotProduct(Q) * invDet;

        // SprawdŸ, czy t jest dodatnie (przeciêcie znajduje siê "przed" promieniem)
        return t > _EPSILON;
    }


    bool rayIntersectsAABB(const Ray& ray, const AABB& box) {
        float tmin = (box.min.x - ray.origin.x) / ray.direction.x;
        float tmax = (box.max.x - ray.origin.x) / ray.direction.x;

        if (tmin > tmax) std::swap(tmin, tmax);

        float tymin = (box.min.y - ray.origin.y) / ray.direction.y;
        float tymax = (box.max.y - ray.origin.y) / ray.direction.y;

        if (tymin > tymax) std::swap(tymin, tymax);

        if ((tmin > tymax) || (tymin > tmax)) {
            return false; // Brak przeciêcia
        }

        if (tymin > tmin) tmin = tymin;
        if (tymax < tmax) tmax = tymax;

        float tzmin = (box.min.z - ray.origin.z) / ray.direction.z;
        float tzmax = (box.max.z - ray.origin.z) / ray.direction.z;

        if (tzmin > tzmax) std::swap(tzmin, tzmax);

        if ((tmin > tzmax) || (tzmin > tmax)) {
            return false; // Brak przeciêcia
        }

        return true; // Promieñ przecina AABB
    }

    bool traverseBVH(int nodeIndex, const Ray& ray) {
        const BVHNode& node = nodes[nodeIndex];

        // 1. Test przeciêcia promienia z AABB
        if (!rayIntersectsAABB(ray, node.bbox)) {
            return false; // Brak przeciêcia, odrzucamy ten wêze³
        }

        bool hitFound = false;

        // 2. Jeœli to liœæ, testuj trójk¹ty
        if (node.leftChild == -1 && node.rightChild == -1) {
            for (int triIndex : node.triangleIndices) {
                const auto& tri = triangles[triIndex];
                float t, u, v;
                if (rayIntersectsTriangle(ray, vertices[tri[0]], vertices[tri[1]], vertices[tri[2]], t, u, v)) {
                    hitFound = true; // Znaleziono przeciêcie
                }
            }
        }
        // 3. Jeœli to wêze³ wewnêtrzny, przeszukaj dzieci
        else {
            if (node.leftChild != -1) {
                hitFound |= traverseBVH(node.leftChild, ray);
            }
            if (node.rightChild != -1) {
                hitFound |= traverseBVH(node.rightChild, ray);
            }
        }

        return hitFound;
    }



private:
    std::vector<BVHNode> nodes;
    int rootNode;

    // Dane trójk¹tów i wierzcho³ków
    const CMesh::Faces& triangles;      // Indeksy wierzcho³ków
    const CMesh::Vertices& vertices;    // Wspó³rzêdne wierzcho³ków

    // Rekurencyjna budowa BVH
    int build(int start, int end, int depth) {
        BVHNode node;

        // Oblicz AABB dla aktualnego zbioru trójk¹tów
        node.bbox = computeAABB(start, end);

        // Jeœli ma³o trójk¹tów, utwórz liœæ
        if (end - start <= 2) {
            for (int i = start; i < end; ++i) {
                node.triangleIndices.push_back(i);
            }
            nodes.push_back(node);
            return nodes.size() - 1;
        }

        // Podziel trójk¹ty na dwa podzbiory
        int mid = (start + end) / 2;
        std::nth_element(triangleIndices.begin() + start, triangleIndices.begin() + mid, triangleIndices.begin() + end,
            [this, depth](int a, int b) {
                return centroid(triangles[a])[depth % 3] < centroid(triangles[b])[depth % 3];
            });

        // Rekurencja dla dzieci
        int leftChild = build(start, mid, depth + 1);
        int rightChild = build(mid, end, depth + 1);

        node.leftChild = leftChild;
        node.rightChild = rightChild;

        nodes.push_back(node);
        return nodes.size() - 1;
    }

    // Obliczenie AABB dla zbioru trójk¹tów
    AABB computeAABB(int start, int end) {
        AABB bbox = {
            CPoint3f{
                std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max()
            },
            CPoint3f{
                -std::numeric_limits<float>::max(),
                -std::numeric_limits<float>::max(),
                -std::numeric_limits<float>::max()
            }
        };

        for (int i = start; i < end; ++i) {
            for (int v = 0; v < 3; v++) {
                bbox.expand(vertices[triangles[i][v]]);
            }
        }
        return bbox;
    }

    // Obliczenie centroidu trójk¹ta
    CPoint3f centroid(const CFace& tri) const {
        return (vertices[tri[0]] + vertices[tri[1]] + vertices[tri[2]]) * (1.0f / 3.0f);
    }

    // Indeksy trójk¹tów do przetwarzania
    std::vector<int> triangleIndices;
};

