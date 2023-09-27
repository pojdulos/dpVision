#pragma once

#include "Workspace.h"
#include "AP.h"

#include "Mesh.h"

//#include "Volumetric.h"

#include <list>

struct info2 {
	INDEX_TYPE parent;
	double krawedz;
	double odleglosc;
	int hits;
};

typedef std::map<INDEX_TYPE, info2> ZnalezieniSasiedzi2; // indeks, odleglosc

class CAnnotationBezier;
class CAnnotationPath;

typedef std::map<CTriple<int>, std::set<INDEX_TYPE>> xyzMapType;
typedef std::map<std::pair<int,int>, std::set<INDEX_TYPE>> xyMapType;


typedef std::pair<CPoint3f, double> CBanka; // œrodek i promieñ
typedef std::vector<CBanka> CWektorBaniek; // indeks ma odpowiadaæ indeksowi œcianki

class CMeshDistance
{
public:
	CMeshDistance(void) {};
	~CMeshDistance(void) {};


	//MeshDistance.cpp
	std::vector<double> meshDistance_Vertex2Face(CModel3D* obj1, CModel3D* obj2, bool useNormals = false, bool useAbsValues = false);
	std::vector<double> meshDistance_Vertex2Face2D(CModel3D* obj1, CModel3D* obj2, bool useNormals = false, bool useAbsValues = false);

	double searchInIndex(CPunkt3D p0, CWektor3D v0, CMesh* m2, xyzMapType &xyzMap, int x, int y, int z, bool useAbsValues = false);
	std::vector<double> meshDistance_Vertex2Face_New(CModel3D* obj1, CModel3D* obj2, bool useNormals = false, bool useAbsValues = false);

	std::vector<double> meshDistance_Vertex2Vertex(CModel3D* obj1, CModel3D* obj2);
	std::vector<double> meshDistance_Vertex2VertexNEW(CModel3D* obj1, CModel3D* obj2);
	std::vector<double> meshDistance_withNormal( CModel3D* obj1, CModel3D* obj2, double maxDistance, bool useNormals, bool useAbsValues=false );
	std::vector<double> meshDistance_withNormal_test(CModel3D* obj1, CModel3D* obj2, double maxDistance, bool useNormals);

	bool myClosestFace(CMesh *m, CPoint3d pkt0, CVector3d vRay, std::set<INDEX_TYPE> podzbior, CPoint3d & IntersectionPoint, INDEX_TYPE & indx, bool onlyFront, bool useNormals);
	bool calcDistance_VNF(CMesh *m2, CPoint3d p0, CVector3d ray, std::set<INDEX_TYPE> &scianki, double maxDistance, double &odleglosc, bool useAbsValues = false);

	double calcGlobalError(CModel3D* obj, std::vector<double> odleglosci, double &wariancja, double lowerLimit = -DBL_MAX, double upperLimit = DBL_MAX);
	double calcGlobalError2(CModel3D* obj, std::vector<double> &odleglosci, double &wariancja, double lowerLimit = -DBL_MAX, double upperLimit = DBL_MAX);
};
