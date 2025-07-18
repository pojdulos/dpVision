#pragma once
#ifndef _RAWDICOMDATA_
#define _RAWDICOMDATA_

#include "Object.h"
//#include "Mesh.h"

#include <map>

typedef unsigned char BYTE;
typedef unsigned short WORD;

class DPVISION_EXPORT RAWdicomData
{

	WORD* m_data;

	inline size_t idx(short x, short y, short z) {
		short xx = (x - m_minC);
		short yy = (y - m_minR);
		short zz = (z - m_minL);

		//(m_dir == RAWdicomData::Direction::FH) ? (z - m_minL) : ((m_lays - 1) - z);

		return ((long)zz * m_cols * m_rows) + ((long)yy * m_cols) + xx;
	};

	inline size_t idx0based(short x, short y, short z) {
		return ((long)z * m_cols * m_rows) + ((long)y * m_cols) + x;
	};

public:
	//enum Direction { HeadToFeetLookingUp, FeetToHeadLookingUp } m_dir;
	short m_cols, m_rows, m_lays;
	short m_minC, m_minR, m_minL;
	int pixelRepresentation;

	std::vector<CPoint3d> m_imgPos;
	std::vector<double> m_gantra;
	CPoint3d m_voxelSize;

	int32_t rescaleIntercept;
	int32_t rescaleSlope;
	int32_t windowCenter;
	int32_t windowWidth;


	RAWdicomData();
	RAWdicomData(const RAWdicomData& src, int flag = 0);

	RAWdicomData(short cols, short rows, short lays, short minC = 0, short minR = 0, short minL = 0);

	~RAWdicomData();

	

	inline bool isValid() { return m_data != nullptr; }
	inline size_t size() { return (size_t)m_lays * m_rows * m_cols; }

	bool init(short cols, short rows, short lays, short minC = 0, short minR = 0, short minL = 0);

	RAWdicomData& operator=(const RAWdicomData& src);

	void copyFrom(const RAWdicomData& src, int flag = 0);

	//WORD& operator()(short x, short y, short z) {
	//	return m_pairs[idx(x, y, z)];
	//};

	inline WORD* layer(short z) { return &m_data[idx(0, 0, z)]; }
	inline WORD getRAW(short x, short y, short z) { return m_data[idx(x, y, z)]; }

	WORD getRescaled(short x, short y, short z, WORD ymin = 0, WORD ymax = 4095); // 16383); // 65535); // 4095);
	

	//WORD get0based(short x, short y, short z) {
	//	if (m_pixRepr == 1)
	//	{
	//		int16_t pix2 = *(int16_t*)&(m_pairs[idx0based(x, y, z)]);
	//		WORD pix = 0x8000 + pix2;

	//		return pix;
	//	}
	//	return m_pairs[idx0based(x, y, z)];
	//};

	//void set(short x, short y, short z, WORD d) {
	//	m_pairs[idx(x, y, z)] = d;
	//};
};


#endif /* _VOLTK_H_BY_DP_INCLUDED_ */