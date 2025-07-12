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

	CAnnotationPodkowa(std::shared_ptr<CBaseObject> m = nullptr);;

	CAnnotationPodkowa( CAnnotationPodkowa &t );;

	~CAnnotationPodkowa(void){};

	//std::list<CPoint3f[3]> getFaces();
	
	void setAngle(int a) { setAnglesDeg( a, 180-a ); };
	virtual int type() { return CAnnotation::PODKOWA; }

	virtual std::wstring getInfoRow();
	virtual std::wstring getTypeWSTR() { return L"podkowa"; };

	virtual void renderSelf() override;

};

