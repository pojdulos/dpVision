#pragma once

#include "Object.h"
#include "RGBA.h"

class __declspec(dllexport) CLidarData : public CObject
{
public:
	typedef struct
	{
		float x,y,z;
		double distance;
		CRGBA color;
	}  Point;
	typedef Point* PointPtr;

protected:
	PointPtr* m_lidarData;
	size_t numCol;
	size_t numRow;
	int renderMode;

	CPoint3f bakMin;
	CPoint3f bakMax;

public:

	CLidarData(void);
	CLidarData(CBaseObject *p);
	~CLidarData(void);

	virtual inline int type() { return CObject::Type::LIDAR; };

	void render();

	unsigned char m_type;
	unsigned char m_alpha;

	void setPoint( size_t col, size_t row, CLidarData::Point point )
	{
		if ( ( numCol >= col ) && ( numRow >= row ) )
		{
			m_lidarData[ col ][ row ] = point;
		}
	}

	CLidarData::PointPtr getPoint( size_t col, size_t row )
	{
		if ( ( numCol >= col ) && ( numRow >= row ) )
		{
			return & m_lidarData[ col ][ row ];
		}
		else
			return NULL;
	}

	CLidarData::Point* getColumn( size_t col )
	{
		if ( numCol >= col )
		{
			return m_lidarData[ col ];
		}
		else
			return NULL;
	}


	void reset( size_t nC, size_t nR )
	{	
		for ( size_t i=0; i<numCol; i++ )
		{
			delete[] m_lidarData[i];
		}
		delete[] m_lidarData;
		numCol = 0;
		numRow = 0;

		bool err = false;

		m_lidarData = new CLidarData::PointPtr[ nC ];

		if (NULL == m_lidarData) return;

		for ( size_t i=0; i<nC; i++ )
		{
			m_lidarData[i] = new CLidarData::Point[ nR ];

			if ( NULL == m_lidarData[i] ) err = true;
		}


		if ( !err )
		{
			numCol=nC;
			numRow=nR;
		}
		else
		{
			for ( size_t i=0; i<nC; i++ )
			{
				if ( NULL != m_lidarData[i] ) delete[] m_lidarData[i];
			}
			delete[] m_lidarData;
			numCol = 0;
			numRow = 0;
		}
	}

	size_t size()
	{
		return numCol * numRow;
	};

	void switchOpt( int key, int val )
	{
		CPoint3f tmp1 = bakMin;
		CPoint3f tmp2 = bakMax;
	

		bakMin = m_min;
		bakMax = m_max;

		if ( renderMode == 0 )
		{
			renderMode = 1;

			m_min.Set( 0, 0, 0 );
			m_max.Set( (float)(numCol-1)/1000.0f, (float)(numRow-1)/1000.0f, 0.0f );
		}
		else
		{
			renderMode = 0;
			m_min = tmp1;
			m_max = tmp2;
		}
	}

};




