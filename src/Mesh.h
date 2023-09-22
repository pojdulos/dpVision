#pragma once
#ifndef _MESH_H_BY_DP_INCLUDED_
#define _MESH_H_BY_DP_INCLUDED_

#include "Object.h"
#include "MeshElements.h"
#include "Triangle.h"
#include "Transform.h"

#include "PointCloud.h"
#include "PolygonGroup.h"

#include <set>

#define	MESHRENDEROPTIONS_NONE 0
#define	MESHRENDEROPTIONS_RENDERTEXTURE 1
#define	MESHRENDEROPTIONS_RENDERSMOOTHVERTEX 2
#define	MESHRENDEROPTIONS_RENDERFACESASPOINTS 4
#define	MESHRENDEROPTIONS_RENDERFACESASEDGES 8
#define	MESHRENDEROPTIONS_RENDERFACESCOLORS 16

class CAnnotationPoints;

class __declspec(dllexport) CMesh : public CPointCloud
{
public:
	//typedef enum {
	//	RENDER_DEFAULT = 0,
	//	RENDER_TEXTURE = 1,
	//	RENDER_SMOOTH_VERTICES = 2,
	//	RENDER_FACES_AS_EDGES = 4,
	//	RENDER_FACES_AS_POINTS = 8
	//} RenderOptions;

	//typedef std::vector<CVertex> Vertices;
	//typedef std::vector<CRGBA> Colors;
	//typedef std::vector<CVector3f> Normals;

	typedef std::vector<CFace> Faces;

	typedef std::vector<SVsplit> Vsplits;

	typedef CSetOfEdges Edges;
	typedef CListOfEdges EdgesCycle;

	typedef std::set< INDEX_TYPE > Neighbors;
	typedef std::map< INDEX_TYPE, CMesh::Neighbors> Neighborhood;

	typedef std::map< INDEX_TYPE, std::set< INDEX_TYPE >> V2Fmap;

	typedef std::map< INDEX_TYPE, std::pair< std::set<INDEX_TYPE>, std::set<INDEX_TYPE> > >  V2Emap;

	//typedef CPointCloud::KDtree KDtree;

	CMesh(CModel3D *p = nullptr);
	CMesh(CMesh &m);
	~CMesh(void);

	static CMesh * fromPointCloud(CPointCloud &cloud);

protected:
	CPolygonGroup m_poly;

private:
	unsigned int iOptions;
	CMesh(CPointCloud &cloud);



	void pushThreeNewFaces(INDEX_TYPE idxTop, INDEX_TYPE idxLeft, INDEX_TYPE idxRight, INDEX_TYPE* idxF, INDEX_TYPE* idxE, std::map<INDEX_TYPE, INDEX_TYPE>* idxMap, CMesh::Faces* tmpf);
	void pushTwoNewFaces(INDEX_TYPE idxTop, INDEX_TYPE idxLeft, INDEX_TYPE idxRight, INDEX_TYPE* idxNewArr, std::map<INDEX_TYPE, INDEX_TYPE>* idxMap, CMesh::Faces* tmpf);
	void pushNewVertex(INDEX_TYPE idx1, INDEX_TYPE idx2, CVertex v, CPointCloud::Vertices* tmpv, CPointCloud::Colors* tmpc, INDEX_TYPE* ret);

public:
	// PMESH EXTENTIONS ------
	CMesh::Vsplits vsplits;
	int version;
	unsigned int orgsize;
	//------------------------

	inline int type() override { return CObject::Type::MESH; };

	virtual CMesh* getCopy() override { return new CMesh(*this); };
	std::wstring infoRow() override;
	void info(std::wstring i[4]) override;

	inline CPolygonGroup &poly() { return m_poly; };

	inline Faces &faces() { return m_poly.faces(); };
	inline Normals &fnormals() { return m_poly.normals(); };
	inline Colors &fcolors() { return m_poly.colors(); };

	inline CFace& face(INDEX_TYPE idx) { return m_poly.faces()[idx]; };
	inline CTriangle getFaceAsTriangle(INDEX_TYPE idx) { return CTriangle(idx, this); };
	inline CVertex& vertex(INDEX_TYPE face_idx, int vid) { return m_vertices[m_poly.faces()[face_idx].toVector()[vid]]; };

	bool calcVN( bool weighted = true );
	bool calcFN();

	virtual void renderSelf() override;
	//virtual void render() override;
	virtual void prepare() override {};

	static int rayTriangleIntersect3D(CPoint3f pA, CPoint3f pB, CPoint3f pC, CVector3f vN, CVector3f vRay, CPoint3f pP0, CPoint3f &pIntersectionPoint, double &pDistance);
	static int rayTriangleIntersect3D(CPoint3d pA, CPoint3d pB, CPoint3d pC, CVector3d vN, CVector3d vRay, CPoint3d pP0, CPoint3d &pIntersectionPoint, double &pDistance);

	int rayTriangleIntersect3D( INDEX_TYPE j, CVector3f vRay, CPoint3f pP0, CPoint3f &pIntersectionPoint, double &pDistance );
	int rayTriangleIntersect3D( INDEX_TYPE j, CVector3d vRay, CPoint3d pP0, CPoint3d &pIntersectionPoint, double &pDistance);

	bool getClosestFace( CPoint3f pkt0, CVector3f vRay, CPoint3f &intersection, INDEX_TYPE &indx, bool onlyFront = true );
	bool getClosestFace( CPoint3f pkt0, CVector3f vRay, std::set<INDEX_TYPE> podzbior, CPoint3f &IntersectionPoint, INDEX_TYPE &indx, bool onlyFront = true );

	int getListOfIntersections( CPoint3f pkt0, CVector3f vRay, std::vector<float> &in, std::vector<float> &out );
	int getListOfIntersections2(CPoint3f pkt0, CVector3f vRay, std::map<INDEX_TYPE,std::pair<float,int>> &in, int ignoreVertex = -1);
	float getInteriorLength(  CPoint3f pkt0, CVector3f vRay, float &maxLen );

	std::vector<CVector3f> &getVectors( CPoint3f pkt0 );

	/* tworzy mapê powi¹zañ indeksów wierzcho³ków z indeksami œcianek */
	void createV2Fmap( CMesh::V2Fmap &map );

	void createV2Emap(CMesh::V2Emap& v2e);

	void createMaps(CMesh::V2Fmap& v2f, CMesh::V2Emap& v2e);
	
	/* szuka wszystkich s¹siadek œcianki i podanym indeksie, s¹siedztwo mo¿e byc po krawêdziach (limit=1) lub po wierzcho³kach (limit=0)*/
	void findNeighborsOfFace( INDEX_TYPE idxF, CMesh::V2Fmap &map, CMesh::Neighbors *sasiadki, int limit = 1);

	/* zwraca uporz¹dkowany zbiór (std::set) œcianek brzegowych dla ca³ej siatki */
	void findBoundaryFaces( std::set<INDEX_TYPE> &brzegowe, CMesh::V2Fmap &map );

	/* zwraca mapê (std::map) œcianek brzegowych wraz z ich s¹siedztwem dla ca³ej siatki */
	void findBoundaryFaces( CMesh::Neighborhood &brzegowe, CMesh::V2Fmap &map);

	/* zwraca uporz¹dkowany zbiór (std::set) œcianek brzegowych dla podanego podzbioru œcianek */
	void findBoundaryFaces( std::set<INDEX_TYPE> &brzegowe, CMesh::V2Fmap &map, std::set< INDEX_TYPE > &scianki);

	/* zwraca mapê (std::map) œcianek brzegowych wraz z ich s¹siedztwem dla podanego podzbioru œcianek */
	void findBoundaryFaces( CMesh::Neighborhood &brzegowe, CMesh::V2Fmap &map, std::set< INDEX_TYPE > &scianki);

	/* szuka krawêdzi brzegowych gdy mamy juz znalezione œcianki brzegowe */
	void findBoundaryEdges( CMesh::Edges &edges, CMesh::Neighborhood sciankiBrzegowe );

	/* zwraca zbiór (std::set) wszystkich krawêdzi w siatce */
	void getEdges( CMesh::Edges &edges);

	/* zwraca zbiór (std::set) wszystkich krawêdzi dla podanego podzbioru œcianek */
	void getEdges( CMesh::Edges &edges, std::set< INDEX_TYPE > &scianki);

	/* zwraca zbiór (std::set) wszystkich krawêdzi brzegowych w siatce */
	void findBoundaryEdges( CMesh::Edges &edges );

	/* zwraca zbiór (std::set) wszystkich krawêdzi brzegowych dla podanego podzbioru œcianek */
	void findBoundaryEdges( CMesh::Edges &edges, std::set< INDEX_TYPE > &scianki );

	void removeDuplicateVertices(double eps = 0.0);
	void removeDuplicateVertices(CMesh::V2Fmap& map, double eps = 0.0);

	std::map<int, CMesh::Neighbors> multiNeighbourhood(CMesh::Neighborhood& map, INDEX_TYPE idx, int dist, double maxDist);

	void createNeighborhoodMapOfVertices( CMesh::Neighborhood &map, bool bidirectional = false );
	void createNeighborhoodMapOfVertices( CMesh::Neighborhood &map, std::set< INDEX_TYPE > &scianki, bool bidirectional=false );

	void transformByMatrixF(float matrix[16]);
	void transformByMatrixD(double matrix[16]);

	void cutPlane(CPlane m_cutPlane, CMesh* reszta = nullptr, CMesh* krawedz = nullptr, CAnnotationPoints* pts = nullptr);

	void correctNormals();
	void invertNormals();
	void removeUnusedVertices();
	
	virtual void applyTransformation(CTransform& from, CTransform& to) override;

	void removeFace(INDEX_TYPE idx);
	void removeFace(CMesh::Faces::iterator itf);
	void removeFace(CMesh::Faces::reverse_iterator itfr);

	INDEX_TYPE addFace(CFace f);
	inline INDEX_TYPE addFace(INDEX_TYPE a, INDEX_TYPE b, INDEX_TYPE c) { return addFace(CFace(a, b, c)); };
	INDEX_TYPE addFace(CFace f, CRGBA fColor);
	inline INDEX_TYPE addFace(INDEX_TYPE a, INDEX_TYPE b, INDEX_TYPE c, unsigned char rc, unsigned char gc, unsigned char bc, unsigned char ac=255 ) { return addFace(CFace(a, b, c),CRGBA(rc,gc,bc,ac)); };
	inline INDEX_TYPE addFace(INDEX_TYPE a, INDEX_TYPE b, INDEX_TYPE c, float rc, float gc, float bc, float ac = 1.0f) { return addFace(CFace(a, b, c), CRGBA(rc, gc, bc, ac)); };
	
	INDEX_TYPE addFaceX(std::vector<INDEX_TYPE>, CRGBA c);

	std::set<std::vector<INDEX_TYPE>*> edgePaths;
	std::set<std::vector<INDEX_TYPE>*> edgeFaces;
	CRGBA selectionFacesColor = CRGBA( 0.0f, 0.8f, 0.0f, 1.0f );
	CRGBA selectionEdgesColor = CRGBA( 1.0f, 0.0f, 0.0f, 1.0f);

	inline bool hasFaceColors() { return m_poly.hasColors(); };
	inline bool hasFaceNormals() { return m_poly.hasNormals(); };

	inline bool hasFaces() { return !m_poly.faces().empty(); };

	inline bool hasTexture() { return getMaterial().hasTexture(); };

	inline bool renderFacesAsEdges() { return 0 != (iOptions & MESHRENDEROPTIONS_RENDERFACESASEDGES); };
	inline bool renderFacesAsEdges(bool b) { b ? (iOptions |= MESHRENDEROPTIONS_RENDERFACESASEDGES) : (iOptions &= ~MESHRENDEROPTIONS_RENDERFACESASEDGES); return b;
	};

	inline bool renderFacesAsPoints() { return 0 != (iOptions & MESHRENDEROPTIONS_RENDERFACESASPOINTS); };
	inline bool renderFacesAsPoints(bool b) { b ? (iOptions |= MESHRENDEROPTIONS_RENDERFACESASPOINTS) : (iOptions &= ~MESHRENDEROPTIONS_RENDERFACESASPOINTS); return b;
	};

	inline bool renderSmoothVertex() { return hasVertexNormals() && ( 0 != (iOptions & MESHRENDEROPTIONS_RENDERSMOOTHVERTEX) ); };
	inline bool renderSmoothVertex(bool b) { b ? (iOptions |= MESHRENDEROPTIONS_RENDERSMOOTHVERTEX) : (iOptions &= ~MESHRENDEROPTIONS_RENDERSMOOTHVERTEX); return b;
	};

	inline bool renderFacesColors() { return m_poly.hasColors() && (0 != (iOptions & MESHRENDEROPTIONS_RENDERFACESCOLORS)); };
	inline bool renderFacesColors(bool b) {
		b ? (iOptions |= MESHRENDEROPTIONS_RENDERFACESCOLORS) : (iOptions &= ~MESHRENDEROPTIONS_RENDERFACESCOLORS); return b;
	};

	inline bool renderWithTexture() { return 0 != (iOptions & MESHRENDEROPTIONS_RENDERTEXTURE); };
	inline bool renderWithTexture(bool b) { b ? (iOptions |= MESHRENDEROPTIONS_RENDERTEXTURE) : (iOptions &= ~MESHRENDEROPTIONS_RENDERTEXTURE); return b;
	};

	virtual void afterLoad(CModel3D* p);

	bool attachTexture(CFileInfo	plikSiatki);

	static CMesh* createPrimitivePlane(double sizeX, double sizeY, int divX=0, int divY=0 );

	// experimental...
	void merge(CMesh*);
};

typedef CMesh* PtrMesh;
typedef CMesh& RefMesh;

#endif /* _MESH_H_BY_DP_INCLUDED_ */