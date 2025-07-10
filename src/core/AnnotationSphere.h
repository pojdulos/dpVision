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

	CAnnotationSphere(std::shared_ptr<CBaseObject> m = nullptr) :Kula(0.0, 0.0, 0.0, 1.0),CAnnotation( m )
	{
		m_lats = m_longs = 128;
		setLabel("sphere");
	};

	CAnnotationSphere( CAnnotationSphere&t ) :Kula(t.center(),t.radius()), CAnnotation( t )
	{
		m_lats = t.m_lats;
		m_longs = t.m_longs;
	};

	CAnnotationSphere( Kula &k ) :Kula(k.center(), k.radius()), CAnnotation(nullptr)
	{
		m_lats = m_longs = 128;
		setLabel("sphere");
	};

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

