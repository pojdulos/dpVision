#pragma once

#include "dll_global.h"

#include "Global.h"

#include "Point3.h"
#include "Vector3.h"
#include "Vertex.h"

#include <cfloat>

class CBoundVector;

class  DPVISION_EXPORT CBoundingBox
{
public:
	typedef enum {
		NullBB,
		InvalidBB
	} InitialValues;

	typedef enum {
		DontRender = 0,
		NotSelected = 1,
		Unlocked = 2,
		Locked = 3,
	} Style;

	void setMin(const CPoint3d &v) { m_min = v; };
	void setMax(const CPoint3d &v) { m_max = v; };

	CPoint3d & getMin() { return m_min; };
	CPoint3d & getMax() { return m_max; };

	void reset(InitialValues v = InitialValues::InvalidBB);

	double dX() { return m_max.X() - m_min.X(); };
	double dY() { return m_max.Y() - m_min.Y(); };
	double dZ() { return m_max.Z() - m_min.Z(); };

	double dX2() { return dX() / 2.0; };
	double dY2() { return dY() / 2.0; };
	double dZ2() { return dZ() / 2.0; };

	CPoint3d getMidpoint() { return (m_min + m_max) / 2.0; }

	bool intersects(CBoundVector r);
	bool intersects(CBoundingBox b);

	static CBoundingBox intersection(CBoundingBox a, CBoundingBox b);
	
	bool isNull()
	{
		return (m_max.X() == m_min.X()) && (m_max.Y() == m_min.Y()) && (m_max.Z() == m_min.Z());
	};

	bool isValid()
	{
		return (m_max.X() >= m_min.X()) && (m_max.Y() >= m_min.Y()) && (m_max.Z() >= m_min.Z()); 
	};

	bool isInvalid()
	{
		return (m_max.X() == -DBL_MAX) || (m_max.Y() == -DBL_MAX) || (m_max.Z() == -DBL_MAX)
			|| (m_min.X() == DBL_MAX) || (m_min.Y() == DBL_MAX) || (m_min.Z() == DBL_MAX)
			|| (m_max.X() < m_min.X()) || (m_max.Y() < m_min.Y()) || (m_max.Z() < m_min.Z());
	};


	void expand(const CBoundingBox b);
	void expand(const CPoint3d p);
	void expand(double value );

	bool contains(const CPoint3d &p);

	//template <typename Container>
	//void expand(Container &container, bool resetIt=false)
	//{
	//	if (resetIt) reset(CBoundingBox::InitialValues::InvalidBB);

	//	for (const auto &v : container) {
	//		expand(v);
	//	}

	//	return bb;
	//}

	bool hit(CPoint3d origin, CVector3d dir, CPoint3d &coord);
	int longest_axis();

	CBoundingBox(InitialValues v = InitialValues::InvalidBB);
	CBoundingBox(const CBoundingBox &bb);
	CBoundingBox(CPoint3d a, CPoint3d b, CPoint3d c);
	CBoundingBox(CPoint3d min, CPoint3d max);
	~CBoundingBox();

	virtual void draw( Style r = DontRender, bool checked = false );
	QVector<CPoint3d> getCorners();
private:
	void drawBox(bool dashed = false);


protected:
	CPoint3d m_min;
	CPoint3d m_max;
	//CVector3d m_whd;
};

