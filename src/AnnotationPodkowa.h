#pragma once
#include "AnnotationCylinder.h"

#include "Punkt3D.h"
#include "Wektor3D.h"
#include "RGBA.h"
#include <list>

struct PFace{
	CPoint3f p[3];
};

class DPVISION_EXPORT CAnnotationPodkowa : public CAnnotationCylinder
{
public:
	double m_arm_length;

	CAnnotationPodkowa( CBaseObject *m = nullptr) :CAnnotationCylinder( m )
	{
		setLabel("podkowa");
		m_arm_length = 50.0;
	};

	CAnnotationPodkowa( CAnnotationPodkowa &t ) :CAnnotationCylinder( t )
	{
		setLabel("podkowa");
		m_arm_length = t.m_arm_length;
	};

	~CAnnotationPodkowa(void){};

	//std::list<CPoint3f[3]> getFaces();
	
	void setAngle(int a) { setAnglesDeg( a, 180-a ); };
	virtual int type() { return CAnnotation::PODKOWA; }

	virtual std::wstring getInfoRow();
	virtual std::wstring getTypeWSTR() { return L"podkowa"; };

	virtual void renderSelf() override;

};

