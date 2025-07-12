#pragma once
#include "Annotation.h"
#include "Kula.h"

#include "Point3.h"
#include "Vector3.h"
#include "RGBA.h"

class DPVISION_EXPORT CAnnotationSphere : public Kula, public CAnnotation
{
public:
	int m_lats;
	int m_longs;

	CAnnotationSphere(std::shared_ptr<CBaseObject> m = nullptr);;

	CAnnotationSphere( CAnnotationSphere&t );;

	CAnnotationSphere( Kula &k );;

	~CAnnotationSphere(void){};

	virtual std::shared_ptr<CBaseObject> getCopy() override
	{
		return std::make_shared<CAnnotationSphere>(*this);
	}

	virtual int type() { return CAnnotation::SPHERE; }

	void applyTransform(CTransform prevT, CTransform newT=CTransform());

	virtual std::wstring getInfoRow();
	virtual std::wstring getTypeWSTR() { return L"sphere"; };

	void drawSphere(int lats, int longs);

	virtual void renderSelf() override;
};

