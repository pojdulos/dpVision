#pragma once
#include "Annotation.h"

#include "Punkt3D.h"
#include "Wektor3D.h"
#include "RGBA.h"

#include <list>

class DPVISION_EXPORT CAnnotationPolygon2D : public CAnnotation
{
public:
	std::list<std::pair<double,double>> m_pairs;

	CAnnotationPolygon2D(std::shared_ptr<CBaseObject> m = nullptr);
	CAnnotationPolygon2D( CAnnotationPolygon2D &p );

	~CAnnotationPolygon2D() { m_pairs.clear(); };

	virtual int type() { return CAnnotation::POLYGON2D; }

	//void addPoint(const CPoint3d &t){ m_pairs.push_back( t ); };
	void addXY(std::pair<double, double> p) { m_pairs.push_back(p); };

	virtual std::wstring getInfoRow();

};

