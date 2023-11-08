#pragma once
#ifndef _VOLDATA_H_BY_DP_INCLUDED_
#define _VOLDATA_H_BY_DP_INCLUDED_

#include "dll_global.h"

#include "Object.h"
//#include "Mesh.h"

#include <map>

struct SVolData {
	typedef struct {
		unsigned char bitLeftX:1;
		unsigned char bitRightX:1;
		unsigned char bitLeftY:1;
		unsigned char bitRightY:1;
		unsigned char bitLeftZ:1;
		unsigned char bitRightZ:1;
	} Neighbourhood;

	Neighbourhood n;

	bool isInside() { return 0 == n.bitLeftX+n.bitRightX+n.bitLeftY+n.bitRightY+n.bitLeftZ+n.bitRightZ; };

	unsigned char c;
};

class CModel3D;

class DPVISION_EXPORT2 CVolData3D : public CObject
{
public:
	typedef SVolData VoxelType;
	typedef std::map<CTriple<int>,CVolData3D::VoxelType> DataType;

	CVolData3D(CBaseObject *p );
	~CVolData3D(void);

	virtual inline int type() { return CObject::Type::VOLUMETRIC; };

	void render();
	void volumRender();
	void drawVoxel( float ix, float iy, float iz, SVolData *v );

	void addVoxel( int ix, int iy, int iz, SVolData *v );
	void addVoxel1( int ix, int iy, int iz, SVolData v );

	size_t size() { return m_data.size(); };
	void clear() { m_data.clear(); };

	unsigned char m_alpha;
	int m_volX, m_volY, m_volZ;
	//CPoint3f m_volMin, m_volMax;

	int m_type;

	CVolData3D::DataType m_data;

protected:
	//_vof4 myFaces;
	//_vof4::iterator face4;

	//void volumAddBox( CMesh &mesh, float ix, float iy, float iz, float w, CRGBA color );
	//void volumReGenerate( CMesh &mesh );
	//void volumPrepare( CMesh &mesh, unsigned int options );
	//void volumRender1( CMesh &mesh, unsigned int options );
};


#endif /* _VOLDATA_H_BY_DP_INCLUDED_ */