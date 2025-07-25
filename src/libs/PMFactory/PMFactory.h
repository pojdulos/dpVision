#pragma once
#ifndef _PMFACTORY_H_BY_DP_INCLUDED_
#define _PMFACTORY_H_BY_DP_INCLUDED_

#include "dll_global.h"

#include "Mesh.h"
#include "Utilities.h"

#include "PMFerrors.h"
#include "PMFfaces.h"

#include "Matrix3x3.h"
#include "Plane.h"

#include <vector>
#include <map>
#include <set>
#include <deque> 

#include "StatusBarManager.h"
#include "MessageBoxManager.h"

struct _sof {
	CFace f;
	CTIndex t;
};

typedef error_type Plane[4];

typedef std::pair<double,double> _uv;

typedef std::map<size_t,CVertex> Vertices;
typedef std::map<size_t,uint8_t> LoF; //Levels of Fixation

typedef std::vector<SVsplit> Vsplits;

typedef std::map<CFace,CTIndex> _mapOfFaces;

typedef std::map<size_t,size_t> TCmap;

typedef std::map<size_t,CTIndex> TextureIndexes;
typedef std::map<size_t,CTCoord> TextureCoords;

typedef std::set<CPoint3f> _setOfPoints;

class DPVISION_EXPORT2 PMFactory
{
public:
	PMFactory(void);
	~PMFactory(void);

	void vSplit( CMesh &dst, unsigned int iNewSize );
	void eColl( CMesh &dst, unsigned int iNewSize );

	//void SavePMT( CMesh &src, const char* nazwa, CVector3f r, CVector3f t );
	//void SaveXML( CMesh &src, const char* nazwa );

	_setOfPoints mSolidPoints;

private:
	_mapOfVertices mVerticesMap;
	_mapOfVertices mTindexesMap;

	bool isSolid( CPoint3f pt );

	Vertices vertices;
	LoF weights;

	CPMFfaces faces;
	Matrices quadrics;		// quadric for each vertex
	CPMFerrors errors;
	Vsplits vsplits;

	TextureIndexes texidxs;
	TextureCoords texcoords;

	unsigned int orgsize;
	bool hasTexture;

	Vertices FixedVertices;

	void Renumerate( CMesh &dst );
	void CMeshToQuadrics( CMesh &src );
	void QuadricsToCMesh( CMesh &dst );
	
	void Reduction(size_t target_num_faces);

	void initial_quadrics();
	void CalcPlane( Plane &p, const CFace &f );

	void select_pair();
	error_type calculate_error(int id_v1, int id_v2, double* vx=0, double* vy=0, double* vz=0);
	// inline methods
	double distance(CVertex v1, CVertex v2);
	double vertex_error(Matrix q, double vx, double vy, double vz);

	// czaszki:
	void SetFixedPoints(void);
	bool IsFixed( CVertex v );
	size_t FindClosestToFixed( CVertex v );

	bool BarycentricTest( CVertex p0, CVertex p1, CVertex p2, CVertex p );
	
	bool projectionOfThePointOnTheFace( CFace f, CVertex p, CVertex &ret );
	void findAllProjections();
};

#endif /* _PMFACTORY_H_BY_DP_INCLUDED_ */