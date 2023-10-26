#pragma once

#ifndef _VOXELS_H_BY_DP_INCLUDED_
#define _VOXELS_H_BY_DP_INCLUDED_

#include "Object.h"

#include "RGBA.h"
#include "Triple.h"
#include <vector>
#include <map>

class __declspec(dllexport) CVoxel
{
public:
	CVoxel();
	CVoxel( CTriple<float> v, CRGBA c=CRGBA( 0.7f, 0.7f, 0.7f, 1.0f ) );
	~CVoxel(){};



	//void render();
	void render( CTriple<float> pos );

	//void setColor(CRGBA color) { m_color.Set(color.R(), color.G(), color.B(), color.A()); };
	void setColor(float r, float g, float b, float a=1.0f) { m_color.SetFloat(r, g, b, a); };
	void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a=255 ) { m_color.Set(r,g,b,a); };

	CRGBA getColor() { return m_color; };

	void setSize(float s) { m_size.Set(s, s, s); };

	int m_type;

protected:
	//CTriple<float> m_pos;
	CRGBA m_color;
	CTriple<float> m_size;

	void drawVoxel(float ix, float iy, float iz);
};


class __declspec(dllexport) CVoxels : public CObject
{
public:
	//typedef std::vector<CVoxel*> DataType;
	typedef std::map<CTriple<float>,CVoxel*> DataType;

	CVoxels(CBaseObject *p );
	~CVoxels() { clear(); };

	virtual inline int type() { return CObject::Type::VOLUMETRIC; };

	CVoxels::DataType *getData() { return &m_voxels; };

	void render();
	size_t size() { return m_voxels.size(); };
	void clear(); 
	//void addVoxel( CVoxel *v ) { m_voxels.push_back( v ); };
	void addVoxel( float x, float y, float z, CVoxel *v ) { m_voxels.insert(CVoxels::DataType::value_type(CTriple<float>(x, y, z), v )); };

protected:
	CVoxels::DataType m_voxels;
};

#endif /* _VOXELS_H_BY_DP_INCLUDED_ */