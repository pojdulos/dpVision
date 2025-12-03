#include "MeshTools.h"

#include <queue>

#include "../api/AP.h"
#include "../api/UI.h"
#include "Mesh.h"

#include "dpLog.h"

void MeshTools::removeInvalidFaces(CMesh& mesh) {
    auto& faces = mesh.faces();
    auto& verts = mesh.vertices();

    size_t initialCount = faces.size();
    std::vector<CFace> newFaces;
    newFaces.reserve(faces.size());

    size_t outOfRangeCount = 0;
    size_t duplicateVertexCount = 0;

    for (const CFace& f : faces) {
        INDEX_TYPE A = f.A(), B = f.B(), C = f.C();

        if (A >= verts.size() || B >= verts.size() || C >= verts.size()) {
            outOfRangeCount++;
            continue; // indeks poza zakresem
        }

        if (A == B || B == C || C == A) {
            duplicateVertexCount++;
            continue; // sklejone wierzcho³ki
        }

        newFaces.push_back(f);
    }

    faces.swap(newFaces);

    size_t removedCount = initialCount - faces.size();
    if (removedCount > 0) {
        dpInfo() << "Removed" << removedCount << "invalid faces ("
            << outOfRangeCount << "out-of-range,"
            << duplicateVertexCount << "duplicate vertices)";
    }
    else {
        dpDebug() << "No invalid faces found";
    }
}

void MeshTools::removeDegenerateFaces(CMesh& mesh, float areaEps) {
    auto& faces = mesh.faces();
    auto& verts = mesh.vertices();

    size_t initialCount = faces.size();
    std::vector<CFace> newFaces;
    newFaces.reserve(faces.size());

    for (const CFace& f : faces) {
        const CVertex& v0 = verts[f.A()];
        const CVertex& v1 = verts[f.B()];
        const CVertex& v2 = verts[f.C()];

        CVector3d e0(v0, v1);
        CVector3d e1(v0, v2);
        float area2 = e0.crossProduct(e1).length(); // 2 * pole

        if (area2 * 0.5f < areaEps)
            continue; // za ma³e pole -> degenerat

        newFaces.push_back(f);
    }

    faces.swap(newFaces);

    size_t removedCount = initialCount - faces.size();
    if (removedCount > 0) {
        dpInfo() << "Removed" << removedCount << "degenerate faces (area <" << areaEps << ")";
    }
    else {
        dpDebug() << "No degenerate faces found";
    }
}


void MeshTools::weldDuplicateVertices(CMesh& mesh, double eps)
{
    size_t initialVertCount = mesh.vertices().size();
    mesh.removeDuplicateVertices(eps);
    size_t finalVertCount = mesh.vertices().size();

    size_t merged = initialVertCount - finalVertCount;
    if (merged > 0) {
        dpInfo() << "Welded" << merged << "duplicate vertices (eps:" << eps << ")";
    }
    else {
        dpDebug() << "No duplicate vertices found";
    }
}

#include <chrono>

void MeshTools::buildEdgeToFaces(const CMesh& mesh,
    std::unordered_map<CEdge, std::vector<INDEX_TYPE>, EdgeHasher>& edgeToFaces)
{
	qInfo() << "MeshTools::buildEdgeToFaces called" << endl;

    const auto& faces = mesh.faces();

    qInfo() << "buildEdgeToFaces: faces=" << faces.size();
    UI::STATUSBAR::setText(QString("Building edge map for %1 faces...").arg(faces.size()));

    if (faces.size() > 5000000) {
        qWarning() << "Mesh is VERY large (" << faces.size()
            << "faces). Using optimized CMesh::getEdges()...";
    }

    edgeToFaces.clear();

    // **U¯YJ NATYWNEJ METODY - std::set jest DU¯O szybszy dla 7M krawêdzi**
    CMesh::Edges meshEdges;  // std::set<CEdge>

    qInfo() << "Calling CMesh::getEdges()...";
    auto startTime = std::chrono::steady_clock::now();
    UI::STATUSBAR::setText("Extracting edges (native method)...");

    const_cast<CMesh&>(mesh).getEdges(meshEdges);

    auto extractTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - startTime).count();

    dpInfo() << "CMesh::getEdges() completed in" << extractTime << "ms ("
        << (extractTime / 1000) << "s), extracted" << meshEdges.size() << "edges";
    UI::STATUSBAR::setText(QString("Extracted %1 edges, converting...").arg(meshEdges.size()));

    // **KONWERSJA: CSetOfEdges -> unordered_map**
    dpInfo() << "Converting to edge-to-faces map...";
    auto convertStart = std::chrono::steady_clock::now();

    edgeToFaces.reserve(meshEdges.size());
    edgeToFaces.max_load_factor(0.5f);

    size_t progressStep = std::max<size_t>(1000, meshEdges.size() / 100);
    size_t processed = 0;

    for (const CEdge& edge : meshEdges) {
        if (++processed % progressStep == 0) {
            double percent = (processed * 100.0) / meshEdges.size();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::steady_clock::now() - convertStart).count();

            UI::STATUSBAR::setText(
                QString("Converting edge %1/%2 (%3%) - %4s")
                .arg(processed).arg(meshEdges.size()).arg((int)percent).arg((int)elapsed)
            );
        }

        // CEdge dziedziczy po std::pair<INDEX_TYPE, INDEX_TYPE>
        INDEX_TYPE v1 = edge.first;
        INDEX_TYPE v2 = edge.second;

        // Kanoniczna forma (mniejszy indeks pierwszy)
        CEdge e = std::minmax(v1, v2);

        // CEdge.faces to std::set<INDEX_TYPE> wszystkich œcian tej krawêdzi
        std::vector<INDEX_TYPE>& faceList = edgeToFaces[e];
        faceList.reserve(edge.faces.size());

        for (INDEX_TYPE faceIdx : edge.faces) {
            faceList.push_back(faceIdx);
        }
    }

    auto convertTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - convertStart).count();

    auto totalTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - startTime).count();

    dpInfo() << "Conversion complete in" << convertTime << "ms";
    dpInfo() << "Total time:" << totalTime << "ms (" << (totalTime / 1000) << "seconds)";
    dpInfo() << "Built edge-to-faces map:" << edgeToFaces.size() << "unique edges";

    UI::STATUSBAR::setText(
        QString("Edge map built: %1 edges in %2s")
        .arg(edgeToFaces.size()).arg(totalTime / 1000)
    );
}

//#include <algorithm>
//#include <execution>
//#include <mutex>

void MeshTools::fixNonManifoldEdges(CMesh& mesh) {
    auto& faces = mesh.faces();
    auto& verts = mesh.vertices();

    size_t initialVertCount = verts.size();

    dpInfo() << "=== FIX NON-MANIFOLD EDGES START ===";
    dpInfo() << "Faces:" << faces.size() << ", Vertices:" << verts.size();
    //UI::STATUSBAR::setText("Fixing non-manifold edges...");

    std::unordered_map<CEdge, std::vector<INDEX_TYPE>, EdgeHasher> edgeToFaces;

    dpInfo() << "Calling buildEdgeToFaces..." << endl;
    //UI::STATUSBAR::setText("Building edge-to-faces map...");

    MeshTools::buildEdgeToFaces(mesh, edgeToFaces);

    dpInfo() << "buildEdgeToFaces returned, map size:" << edgeToFaces.size();
    UI::STATUSBAR::setText("Identifying non-manifold edges...");

    // Faza 1: Identyfikacja non-manifold edges
    dpInfo() << "Starting identification phase...";

    std::vector<std::pair<CEdge, std::vector<INDEX_TYPE>>> nonManifoldEdges;
    nonManifoldEdges.reserve(edgeToFaces.size() / 100);

    size_t processed = 0;
    size_t progressStep = std::max<size_t>(1, edgeToFaces.size() / 100);
    auto startTime = std::chrono::steady_clock::now();

    for (const auto& kv : edgeToFaces) {
        if (++processed % progressStep == 0) {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - startTime).count();

            double percent = (processed * 100.0) / edgeToFaces.size();

            UI::STATUSBAR::setText(
                QString("Scanning edges: %1/%2 (%3%) - found %4 non-manifold")
                .arg(processed).arg(edgeToFaces.size())
                .arg((int)percent).arg(nonManifoldEdges.size())
            );

            dpInfo() << "Scanned" << processed << "/" << edgeToFaces.size()
                << "(" << (int)percent << "%) - found"
                << nonManifoldEdges.size() << "non-manifold edges";
        }

        if (kv.second.size() > 2) {
            nonManifoldEdges.emplace_back(kv.first, kv.second);
        }
    }

    dpInfo() << "Identification complete in"
        << std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - startTime).count()
        << "ms";

    if (nonManifoldEdges.empty()) {
        dpInfo() << "No non-manifold edges found";
        UI::STATUSBAR::setText("No non-manifold edges found");
        return;
    }

    dpInfo() << "Found" << nonManifoldEdges.size() << "non-manifold edges";
    UI::STATUSBAR::setText(QString("Found %1 non-manifold edges").arg(nonManifoldEdges.size()));

    // Faza 2: Przygotowanie zmian (szybkie, bez alokacji)
    dpDebug() << "Preparing vertex duplicates...";

    struct FaceUpdate {
        INDEX_TYPE faceIdx;
        INDEX_TYPE oldU, oldV;
        INDEX_TYPE newU, newV;
    };

    std::vector<FaceUpdate> allUpdates;
    size_t estimatedUpdates = 0;

    // Policz ile bêdzie update'ów
    for (const auto& pair : nonManifoldEdges) {
        estimatedUpdates += (pair.second.size() - 2);
    }

    allUpdates.reserve(estimatedUpdates);
    size_t totalVertsToAdd = estimatedUpdates * 2;

    dpDebug() << "Allocating" << totalVertsToAdd << "new vertices...";

    // Faza 3: Rezerwacja miejsca dla nowych wierzcho³ków
    size_t nextVertIdx = verts.size();
    verts.reserve(verts.size() + totalVertsToAdd);

    // Przygotuj wszystkie update'y
    for (const auto& pair : nonManifoldEdges) {
        const CEdge& e = pair.first;
        const auto& incidentFaces = pair.second;

        INDEX_TYPE u = std::min(e.first, e.second);
        INDEX_TYPE v = std::max(e.first, e.second);

        // Ka¿da nadmiarowa œciana (od 3. wzwy¿) potrzebuje nowych wierzcho³ków
        for (size_t i = 2; i < incidentFaces.size(); ++i) {
            FaceUpdate upd;
            upd.faceIdx = incidentFaces[i];
            upd.oldU = u;
            upd.oldV = v;
            upd.newU = nextVertIdx++;
            upd.newV = nextVertIdx++;
            allUpdates.push_back(upd);
        }
    }

    // Faza 4: Duplikacja wierzcho³ków (bulk insert)
    dpDebug() << "Duplicating vertices...";

    for (const auto& upd : allUpdates) {
        verts.push_back(verts[upd.oldU]);
        verts.push_back(verts[upd.oldV]);
    }

    // Faza 5: Aktualizacja œcian
    dpDebug() << "Updating" << allUpdates.size() << "face indices...";

    for (const auto& upd : allUpdates) {
        CFace& f = faces[upd.faceIdx];
        INDEX_TYPE A = f.A();
        INDEX_TYPE B = f.B();
        INDEX_TYPE C = f.C();
        INDEX_TYPE u = upd.oldU;
        INDEX_TYPE v = upd.oldV;
        INDEX_TYPE uNew = upd.newU;
        INDEX_TYPE vNew = upd.newV;

        if ((A == u && B == v) || (A == v && B == u)) {
            f = CFace(uNew, vNew, C);
        }
        else if ((B == u && C == v) || (B == v && C == u)) {
            f = CFace(A, uNew, vNew);
        }
        else if ((C == u && A == v) || (C == v && A == u)) {
            f = CFace(vNew, B, uNew);
        }
        else {
            dpWarn() << "Edge detected but vertex pair not found in face" << upd.faceIdx;
        }
    }

    dpInfo() << "Fixed" << nonManifoldEdges.size() << "non-manifold edges, added"
        << totalVertsToAdd << "vertices (" << initialVertCount << "->"
        << verts.size() << ")";
}

void MeshTools::buildVertexToFaces(const CMesh& mesh,
    std::vector<std::vector<INDEX_TYPE>>& vertexFaces)
{
    const auto& faces = mesh.faces();
    size_t nV = mesh.vertices().size();
    vertexFaces.assign(nV, {});

    for (INDEX_TYPE fi = 0; fi < (INDEX_TYPE)faces.size(); ++fi) {
        const CFace& f = faces[fi];
        vertexFaces[f.A()].push_back(fi);
        vertexFaces[f.B()].push_back(fi);
        vertexFaces[f.C()].push_back(fi);
    }

    dpDebug() << "Built vertex-to-faces map for" << nV << "vertices";
}


void MeshTools::fixNonManifoldVertices(CMesh& mesh) {
    auto& faces = mesh.faces();
    auto& verts = mesh.vertices();

    size_t initialVertCount = verts.size();

    dpInfo() << "=== FIX NON-MANIFOLD VERTICES START ===";
    dpInfo() << "Vertices:" << verts.size() << ", Faces:" << faces.size();
    UI::STATUSBAR::setText("Building vertex-to-faces map...");

    // **U¯YJ NATYWNEJ METODY z CMesh**
    CMesh::V2Fmap vertexFaces;
    const_cast<CMesh&>(mesh).createV2Fmap(vertexFaces);

    dpInfo() << "Built V2Fmap for" << vertexFaces.size() << "vertices (only used)";
    UI::STATUSBAR::setText("Analyzing non-manifold vertices...");

    std::vector<char> visitedFace;
    visitedFace.resize(faces.size());

    size_t nonManifoldVertCount = 0;
    size_t verticesAdded = 0;
    size_t totalComponents = 0;

    // **PROGRESS REPORTING**
    size_t processedVerts = 0;
    size_t progressStep = std::max<size_t>(1000, vertexFaces.size() / 100);
    auto startTime = std::chrono::steady_clock::now();

    // **ITERUJ PO MAPIE zamiast po wszystkich wierzcho³kach**
    for (const auto& vf : vertexFaces) {
        INDEX_TYPE v = vf.first;
        const auto& incFaces = vf.second;  // std::set<INDEX_TYPE>

        // **PROGRESS**
        if (++processedVerts % progressStep == 0) {
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::steady_clock::now() - startTime).count();

            double percent = (processedVerts * 100.0) / vertexFaces.size();

            UI::STATUSBAR::setText(
                QString("Analyzing vertex %1/%2 (%3%) - found %4 non-manifold - %5s")
                .arg(processedVerts).arg(vertexFaces.size())
                .arg((int)percent).arg(nonManifoldVertCount).arg((int)elapsed)
            );

            dpInfo() << "Progress:" << processedVerts << "/" << vertexFaces.size()
                << "(" << (int)percent << "%) - found" << nonManifoldVertCount
                << "non-manifold vertices";
        }

        if (incFaces.size() <= 1) continue;

        // Reset visited
        std::fill(visitedFace.begin(), visitedFace.end(), 0);

        std::vector<std::vector<INDEX_TYPE>> components;

        // ZnajdŸ spójne sk³adowe w 1-ringu tego wierzcho³ka
        for (INDEX_TYPE fi : incFaces) {
            if (visitedFace[fi]) continue;

            std::vector<INDEX_TYPE> comp;
            std::queue<INDEX_TYPE> q;
            q.push(fi);
            visitedFace[fi] = 1;

            while (!q.empty()) {
                INDEX_TYPE fIdx = q.front();
                q.pop();
                comp.push_back(fIdx);

                const CFace& f = faces[fIdx];

                // Szukaj s¹siadów w incFaces
                for (INDEX_TYPE fj : incFaces) {
                    if (visitedFace[fj]) continue;

                    const CFace& g = faces[fj];

                    // SprawdŸ czy dziel¹ krawêdŸ (2 wspólne wierzcho³ki)
                    int common = 0;
                    INDEX_TYPE a1[3] = { f.A(), f.B(), f.C() };
                    INDEX_TYPE a2[3] = { g.A(), g.B(), g.C() };

                    for (int ii = 0; ii < 3; ++ii) {
                        for (int jj = 0; jj < 3; ++jj) {
                            if (a1[ii] == a2[jj]) {
                                ++common;
                                break;
                            }
                        }
                    }

                    if (common >= 2) { // Wspólna krawêdŸ
                        visitedFace[fj] = 1;
                        q.push(fj);
                    }
                }
            }

            components.push_back(std::move(comp));
        }

        if (components.size() <= 1)
            continue; // Manifold

        nonManifoldVertCount++;
        totalComponents += components.size();

        dpDebug() << "Non-manifold vertex" << v << "split into"
            << components.size() << "components";

        // Rozdziel v na tyle wierzcho³ków, ile jest komponentów
        for (size_t ci = 1; ci < components.size(); ++ci) {
            INDEX_TYPE newV = (INDEX_TYPE)verts.size();
            verts.push_back(verts[v]);
            verticesAdded++;

            for (INDEX_TYPE fIdx : components[ci]) {
                CFace& f = faces[fIdx];
                if (f.A() == v) f.A(newV);
                if (f.B() == v) f.B(newV);
                if (f.C() == v) f.C(newV);
            }
        }
    }

    auto totalTime = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - startTime).count();

    if (nonManifoldVertCount > 0) {
        dpInfo() << "Fixed" << nonManifoldVertCount << "non-manifold vertices ("
            << totalComponents << "components), added" << verticesAdded
            << "vertices (" << initialVertCount << "->" << verts.size()
            << ") in" << totalTime << "seconds";
    }
    else {
        dpDebug() << "No non-manifold vertices found (checked in" << totalTime << "s)";
    }

    UI::STATUSBAR::setText("Non-manifold vertices fixed");
}

void MeshTools::removeIsolatedVertices(CMesh& mesh) {
    auto& faces = mesh.faces();
    auto& verts = mesh.vertices();

    size_t initialCount = verts.size();
    std::vector<char> used(verts.size(), 0);
    for (const CFace& f : faces) {
        used[f.A()] = 1;
        used[f.B()] = 1;
        used[f.C()] = 1;
    }

    std::vector<INDEX_TYPE> remap(verts.size(), (INDEX_TYPE)-1);
    std::vector<CVertex> newVerts;
    newVerts.reserve(verts.size());

    for (INDEX_TYPE i = 0; i < (INDEX_TYPE)verts.size(); ++i) {
        if (!used[i]) continue;
        remap[i] = (INDEX_TYPE)newVerts.size();
        newVerts.push_back(verts[i]);
    }

    for (CFace& f : faces) {
        f.A(remap[f.A()]);
        f.B(remap[f.B()]);
        f.C(remap[f.C()]);
    }

    verts.swap(newVerts);

    size_t removedCount = initialCount - verts.size();
    if (removedCount > 0) {
        dpInfo() << "Removed" << removedCount << "isolated vertices ("
            << initialCount << "->" << verts.size() << ")";
    }
    else {
        dpDebug() << "No isolated vertices found";
    }
}

#include <unordered_set>


// Funkcja oblicza aspect ratio trójk¹ta (stosunek najd³u¿szej krawêdzi do wysokoœci)
float calculateAspectRatio(const CVertex& v0, const CVertex& v1, const CVertex& v2) {
    CVector3d e0 = CVector3d(v0, v1);
    CVector3d e1 = CVector3d(v1, v2);
    CVector3d e2 = CVector3d(v2, v0);

    float len0 = e0.length();
    float len1 = e1.length();
    float len2 = e2.length();

    float longest = std::max({ len0, len1, len2 });
    float perimeter = len0 + len1 + len2;

    // Pole trójk¹ta za pomoc¹ wzoru Herona
    float s = perimeter / 2.0f;
    float area = std::sqrt(s * (s - len0) * (s - len1) * (s - len2));

    if (area < 1e-6f) return 1000.0f; // Zdegenerowany trójk¹t

    // Wysokoœæ z najd³u¿szej krawêdzi
    float height = 2.0f * area / longest;

    return longest / height; // Aspect ratio
}


void MeshTools::subdivideNarrowFaces(std::shared_ptr<CMesh> mesh,
    float aspectRatioThreshold = 5.0f,
    int maxIterations = 3)
{
    if (!mesh) {
        dpError() << "subdivideNarrowFaces: null mesh pointer";
        return;
    }

    //using Edge = std::pair<INDEX_TYPE, INDEX_TYPE>;

    UI::STATUSBAR::setText("Subdividing narrow faces...");
    dpInfo() << "=== SUBDIVIDE NARROW FACES: START ===";
    dpInfo() << "Initial faces:" << mesh->faces().size()
        << ", vertices:" << mesh->vertices().size();
    dpInfo() << "Aspect ratio threshold:" << aspectRatioThreshold;

    for (int iteration = 0; iteration < maxIterations; ++iteration) {
        dpInfo() << "Iteration" << (iteration + 1) << "/" << maxIterations;

        std::vector<CFace>& faces = mesh->faces();
        std::vector<CVertex>& verts = mesh->vertices();

        const size_t faceCount = faces.size();
        if (faceCount == 0) {
            dpWarn() << "No faces to process";
            break;
        }

        // 1) Wyznaczamy zbiór krawêdzi do podzia³u (edge-based)
        std::unordered_set<CEdge, EdgeHasher> edgesToSplit;
        edgesToSplit.reserve(faceCount * 2);

        for (INDEX_TYPE i = 0; i < static_cast<INDEX_TYPE>(faceCount); ++i) {
            const CFace& f = faces[i];
            const CVertex& v0 = verts[f.A()];
            const CVertex& v1 = verts[f.B()];
            const CVertex& v2 = verts[f.C()];

            float aspect = calculateAspectRatio(v0, v1, v2);
            if (aspect <= aspectRatioThreshold)
                continue;

            CVector3d e0(v0, v1), e1(v1, v2), e2(v2, v0);
            float len0 = e0.length();
            float len1 = e1.length();
            float len2 = e2.length();

            CEdge eAB = std::minmax(f.A(), f.B());
            CEdge eBC = std::minmax(f.B(), f.C());
            CEdge eCA = std::minmax(f.C(), f.A());

            // Sortuj krawêdzie wg d³ugoœci (malej¹co)
            std::vector<std::pair<float, CEdge>> sortedEdges = {
                {len0, eAB}, {len1, eBC}, {len2, eCA}
            };
            std::sort(sortedEdges.begin(), sortedEdges.end(),
                [](const auto& a, const auto& b) {
                    return a.first > b.first;
                });

            float longest = sortedEdges[0].first;
            float secondLongest = sortedEdges[1].first;
            float shortest = sortedEdges[2].first;

            // Jeœli 2 d³ugie krawêdzie (ró¿nica > 50%), oznacz obie
            if (secondLongest > shortest * 1.5f) {
                edgesToSplit.insert(sortedEdges[0].second);
                edgesToSplit.insert(sortedEdges[1].second);
            }
            else {
                edgesToSplit.insert(sortedEdges[0].second);
            }
        }

        if (edgesToSplit.empty()) {
            dpInfo() << "No narrow faces above threshold, stopping early.";
            break;
        }

        dpInfo() << "Edges to split:" << edgesToSplit.size();

        // 2) W³aœciwa subdivizja
        std::unordered_map<CEdge, INDEX_TYPE, EdgeHasher> edgeMidpoints;
        edgeMidpoints.reserve(edgesToSplit.size());

        std::vector<CFace> newFaces;
        newFaces.reserve(faceCount * 2);

        auto getMidpoint = [&](const CEdge& e, INDEX_TYPE i0, INDEX_TYPE i1) -> INDEX_TYPE {
            auto it = edgeMidpoints.find(e);
            if (it != edgeMidpoints.end())
                return it->second;

            const CVertex& a = verts[i0];
            const CVertex& b = verts[i1];

            CVertex mid;
            mid.x = (a.x + b.x) * 0.5f;
            mid.y = (a.y + b.y) * 0.5f;
            mid.z = (a.z + b.z) * 0.5f;

            INDEX_TYPE midIdx = static_cast<INDEX_TYPE>(verts.size());
            verts.push_back(mid);
            edgeMidpoints[e] = midIdx;
            return midIdx;
            };

        int subdivided = 0;
        int count1 = 0, count2 = 0, count3 = 0; // Statystyki

        for (INDEX_TYPE i = 0; i < static_cast<INDEX_TYPE>(faceCount); ++i) {
            const CFace& f = faces[i];

            INDEX_TYPE A = f.A();
            INDEX_TYPE B = f.B();
            INDEX_TYPE C = f.C();

            CEdge eAB = std::minmax(A, B);
            CEdge eBC = std::minmax(B, C);
            CEdge eCA = std::minmax(C, A);

            bool splitAB = edgesToSplit.count(eAB) > 0;
            bool splitBC = edgesToSplit.count(eBC) > 0;
            bool splitCA = edgesToSplit.count(eCA) > 0;

            int splitCount = (splitAB ? 1 : 0) + (splitBC ? 1 : 0) + (splitCA ? 1 : 0);

            if (splitCount == 0) {
                newFaces.push_back(f);
                continue;
            }

            subdivided++;

            if (splitCount == 1) {
                count1++;
                if (splitAB) {
                    INDEX_TYPE mAB = getMidpoint(eAB, A, B);
                    newFaces.emplace_back(A, mAB, C);
                    newFaces.emplace_back(mAB, B, C);
                }
                else if (splitBC) {
                    INDEX_TYPE mBC = getMidpoint(eBC, B, C);
                    newFaces.emplace_back(B, mBC, A);
                    newFaces.emplace_back(mBC, C, A);
                }
                else {
                    INDEX_TYPE mCA = getMidpoint(eCA, C, A);
                    newFaces.emplace_back(C, mCA, B);
                    newFaces.emplace_back(mCA, A, B);
                }

            }
            else if (splitCount == 2) {
                count2++;
                if (splitAB && splitBC) {
                    INDEX_TYPE mAB = getMidpoint(eAB, A, B);
                    INDEX_TYPE mBC = getMidpoint(eBC, B, C);
                    newFaces.emplace_back(A, mAB, C);
                    newFaces.emplace_back(mAB, mBC, C);
                    newFaces.emplace_back(mAB, B, mBC);

                }
                else if (splitBC && splitCA) {
                    INDEX_TYPE mBC = getMidpoint(eBC, B, C);
                    INDEX_TYPE mCA = getMidpoint(eCA, C, A);
                    newFaces.emplace_back(B, mBC, A);
                    newFaces.emplace_back(mBC, mCA, A);
                    newFaces.emplace_back(mBC, C, mCA);

                }
                else {
                    INDEX_TYPE mCA = getMidpoint(eCA, C, A);
                    INDEX_TYPE mAB = getMidpoint(eAB, A, B);
                    newFaces.emplace_back(C, mCA, B);
                    newFaces.emplace_back(mCA, mAB, B);
                    newFaces.emplace_back(mCA, A, mAB);
                }

            }
            else {
                count3++;
                INDEX_TYPE mAB = getMidpoint(eAB, A, B);
                INDEX_TYPE mBC = getMidpoint(eBC, B, C);
                INDEX_TYPE mCA = getMidpoint(eCA, C, A);

                newFaces.emplace_back(A, mAB, mCA);
                newFaces.emplace_back(B, mBC, mAB);
                newFaces.emplace_back(C, mCA, mBC);
                newFaces.emplace_back(mAB, mBC, mCA);
            }
        }

        dpInfo() << "Subdivided:" << subdivided
            << " (1-edge:" << count1 << ", 2-edge:" << count2 << ", 3-edge:" << count3 << ")"
            << ", New faces:" << newFaces.size()
            << ", Vertices:" << verts.size();

        faces = std::move(newFaces);

        if (subdivided == 0) {
            dpWarn() << "No faces were subdivided in this iteration";
            break;
        }
    }

    dpInfo() << "Final faces:" << mesh->faces().size()
        << ", vertices:" << mesh->vertices().size();
    dpInfo() << "=== SUBDIVIDE NARROW FACES: END ===";
    UI::STATUSBAR::setText("Subdivision complete");
}



void MeshTools::repairMesh(CMesh& mesh)
{
    size_t initialFaces = mesh.faces().size();
    size_t initialVerts = mesh.vertices().size();

    dpInfo() << "=== MESH REPAIR: START ===";
    dpInfo() << "Initial state: faces=" << initialFaces << ", vertices=" << initialVerts;

    dpInfo() << "Step 1/7: removeInvalidFaces...";
    MeshTools::removeInvalidFaces(mesh);

    dpInfo() << "Step 2/7: removeDegenerateFaces...";
    MeshTools::removeDegenerateFaces(mesh, 1.0E-10F);

    dpInfo() << "Step 3/7: weldDuplicateVertices...";
    MeshTools::weldDuplicateVertices(mesh, 0.0);

    dpInfo() << "Step 4/7: removeDegenerateFaces (again)...";
    MeshTools::removeDegenerateFaces(mesh);

    dpInfo() << "Step 5/7: fixNonManifoldEdges...";
    MeshTools::fixNonManifoldEdges(mesh);

    dpInfo() << "Step 6/7: fixNonManifoldVertices...";
    MeshTools::fixNonManifoldVertices(mesh);

    dpInfo() << "Step 7/7: removeIsolatedVertices...";
    MeshTools::removeIsolatedVertices(mesh);

    size_t finalFaces = mesh.faces().size();
    size_t finalVerts = mesh.vertices().size();

    dpInfo() << "Final state: faces=" << finalFaces << " (delta:"
        << static_cast<int>(finalFaces - initialFaces) << "), vertices="
        << finalVerts << " (delta:" << static_cast<int>(finalVerts - initialVerts) << ")";
    dpInfo() << "=== MESH REPAIR: END ===";
}

