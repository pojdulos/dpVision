#pragma once

#include "Global.h"
#include "Point3.h"
#include "Mesh.h"
#include "Volumetric.h"

#include "dll_global.h"

typedef CPoint3d GLvector;

struct GLTriangle
{
    CVertex vertex[3];
};

class DPVISION_EXPORT MeshMaker
{
public:
    MeshMaker(Volumetric* volTK, CMesh* mesh, int pMin, int pMax) : m_volTK(volTK), m_mesh(mesh), progmin(pMin), progmax(pMax), DIVIDER(1) {}
    ~MeshMaker() {}

    std::vector<GLTriangle> triangles;
    CMesh* m_mesh;

private:
    int progmin;
    int progmax;

    double DIVIDER;

    Volumetric* m_volTK;

    /*
        getOffset finds the approximate point of intersection of the surface
        between two points with the values fValue1 and fValue2
    */
    double getOffset(double, double, double);

    Volumetric::VoxelType getVoxelValue(CPoint3d vox);
    Volumetric::VoxelType getVoxelValue(short, short, short);

    /*
        vMarchTetrahedron performs the Marching Tetrahedrons algorithm on a single tetrahedron
    */
    void vMarchTetrahedron(GLvector* pasTetrahedronPosition, double* pafTetrahedronValue);
    
    /*
        vMarchCube performs the Marching Tetrahedrons algorithm on a single cube by making six calls to vMarchTetrahedron
    */
    void vMarchCube(CPoint3d position);

    //void vGetColor(GLvector& rfColor, GLvector& rfPosition, GLvector& rfNormal);
    //void vNormalizeVector(GLvector& rfVectorResult, GLvector& rfVectorSource);
    //void vGetNormal(GLvector& rfNormal, double fX, double fY, double fZ);
    void vMarchCube1(CPoint3d position);


public:
    //void MarchingTetrahedronBAK(long* progress, int quality, std::string logPath);
    //vMarchingCubes iterates over the entire dataset, calling vMarchCube on each cube
    void MarchingTetrahedron(double div=0);
    void MarchingCube(double div = 0);
};
