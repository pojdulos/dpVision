#pragma once
#include "Annotation.h"

#include "Punkt3D.h"
#include "Wektor3D.h"
#include "RGBA.h"

#include <list>

class __declspec(dllexport)  CAnnotationPolygon : public CAnnotation
{
	std::list<CPoint3d> m_data;

public:
	CAnnotationPolygon( CBaseObject *m = nullptr) :CAnnotation(m)
	{
		setLabel("polygon");
	};

	CAnnotationPolygon( CAnnotationPolygon &p ) :CAnnotation(p)
	{
		setLabel("polygon");
		m_data = p.m_data;
	}

	~CAnnotationPolygon() { m_data.clear(); };

	virtual int type() { return CAnnotation::POLYGON; }

	void addPoint(const CPoint3d &t){ m_data.push_back( t ); };
	void addPoint(double x, double y, double z) { m_data.push_back(CPoint3d(x,y,z)); };

	virtual std::wstring getInfoRow();

	virtual void renderSelf() override;
};

