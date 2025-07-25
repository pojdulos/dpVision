#pragma once
#include "Annotation.h"

#include "Punkt3D.h"
#include "Wektor3D.h"
#include "RGBA.h"

#include <list>

class DPVISION_EXPORT CAnnotationPolygon : public CAnnotation
{
public:
	std::list<CPoint3d> m_data;

	CAnnotationPolygon(std::shared_ptr<CBaseObject> m = nullptr);
	CAnnotationPolygon( CAnnotationPolygon &p );

	~CAnnotationPolygon() { m_data.clear(); };

	virtual int type() { return CAnnotation::POLYGON; }

	void addPoint(const CPoint3d &t){ m_data.push_back( t ); };
	void addPoint(double x, double y, double z) { m_data.push_back(CPoint3d(x,y,z)); };

	virtual std::wstring getInfoRow();

};

