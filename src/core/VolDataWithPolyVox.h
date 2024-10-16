#pragma once
#ifndef _VOLDATA_H_BY_DP_INCLUDED_
#define _VOLDATA_H_BY_DP_INCLUDED_

#include "Mesh.h"

/*
struct SVolData {
	CRGBA color;
	unsigned type : 4;
	unsigned nLeftX : 1;
	unsigned nRightX : 1;
	unsigned nLeftY : 1;
	unsigned nRightY : 1;
	unsigned nLeftZ : 1;
	unsigned nRightZ : 1;
};
*/

struct SVolData {
	enum {
		bitLeftX = 0x01,
		bitRightX = 0x02,
		bitLeftY = 0x04,
		bitRightY = 0x08,
		bitLeftZ = 0x10,
		bitRightZ = 0x20
	};

	unsigned char n;

	unsigned char sum() { return n; };

	unsigned char c;
	//unsigned char r;
	//unsigned char g;
	//unsigned char b;
};

class CModel3D;

//class CVolData3D : public std::vector<CVolData2D>
class CVolData3D : public std::map<Trio<int>,SVolData>
{
public:
	CVolData3D( CModel3D *p );
	~CVolData3D(void);
	void prepare( CMesh &mesh, unsigned int options = MESHRENDEROPTIONS_NONE );
	void render( CMesh& mesh, unsigned int options = MESHRENDEROPTIONS_NONE );
	void volumAddBox( CMesh &mesh, float ix, float iy, float iz, float w, CRGBA color );
	void volumReGenerate( CMesh &mesh );
	void volumPrepare( CMesh &mesh, unsigned int options );
	void volumRender1( CMesh &mesh, unsigned int options );
	void volumRender( CMesh &mesh, unsigned int options );

	void addVoxel( int ix, int iy, int iz, SVolData *v );
	void addVoxel1( int ix, int iy, int iz, SVolData *v );
	void drawVoxel( float ix, float iy, float iz, SVolData *v );

	CModel3D *m_parent;

	unsigned char m_alpha;
	int m_volX, m_volY, m_volZ;
	CPunkt3D m_volMin, m_volMax;

	int m_type;

	_vof4 myFaces;
	_vof4::iterator face4;
};


#endif /* _VOLDATA_H_BY_DP_INCLUDED_ */