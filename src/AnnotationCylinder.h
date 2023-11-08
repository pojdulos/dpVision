#pragma once
#include "Annotation.h"

#include "Punkt3D.h"
#include "Wektor3D.h"
#include "RGBA.h"

class DPVISION_EXPORT CAnnotationCylinder : public CAnnotation
{
public:
	CPoint3f m_point;
	float m_radius, m_halfLength;
	double m_start_angle, m_end_angle;

	CAnnotationCylinder( CBaseObject *m = nullptr) :CAnnotation( m )
	{
		setLabel("cylinder");
		m_point = CPoint3f(0,0,0);
		m_radius = 0;
		m_halfLength = 0;
		m_start_angle = 0.0;
		m_end_angle = 360.0;
	};

	CAnnotationCylinder( CAnnotationCylinder &t ) :CAnnotation( t )
	{
		setLabel("cylinder");
		m_point = t.m_point;
		m_radius = t.m_radius;
		m_halfLength = t.m_halfLength;
		m_start_angle = t.m_start_angle;
		m_end_angle = t.m_end_angle;
	};

	~CAnnotationCylinder(void){};

	virtual int type() { return CAnnotation::CYLINDER; }

	void setPoint( const CTriple<float> &t ){ m_point.Set( t ); };
	void setPoint( const float &tx, const float &ty, const float &tz ){ m_point.Set( tx, ty, tz ); };
	CPoint3f getPoint(){ return m_point; };
	float X() { return m_point.X(); };
	float Y() { return m_point.Y(); };
	float Z() { return m_point.Z(); };

	void setAnglesDeg( int start, int end)
	{
		m_start_angle = start;
		m_end_angle = end;
	}

	virtual std::wstring getInfoRow();
	virtual std::wstring getTypeWSTR() { return L"cylinder"; };

	virtual void renderSelf() override;

};

