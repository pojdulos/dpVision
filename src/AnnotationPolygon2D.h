#pragma once
#include "Annotation.h"

#include "Punkt3D.h"
#include "Wektor3D.h"
#include "RGBA.h"

#include <list>

class DPVISION_EXPORT CAnnotationPolygon2D : public CAnnotation
{
	std::list<std::pair<double,double>> m_data;

public:
	CAnnotationPolygon2D( CBaseObject *m = nullptr) :CAnnotation(m)
	{
		setLabel("polygon2d");
	};

	CAnnotationPolygon2D( CAnnotationPolygon2D &p ) :CAnnotation(p)
	{
		setLabel("polygon2d");
		m_data = p.m_data;
	}

	~CAnnotationPolygon2D() { m_data.clear(); };

	virtual int type() { return CAnnotation::POLYGON2D; }

	//void addPoint(const CPoint3d &t){ m_data.push_back( t ); };
	void addXY(std::pair<double, double> p) { m_data.push_back(p); };

	virtual std::wstring getInfoRow();

	virtual void renderSelf() override;
};

