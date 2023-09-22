#pragma once
#include "Annotation.h"
#include "Kula.h"

#include "Point3.h"
#include "Vector3.h"
#include "RGBA.h"

class __declspec(dllexport)  CAnnotationSphere : public Kula, public CAnnotation
{
public:
	int m_lats;
	int m_longs;

	CAnnotationSphere( CBaseObject *m = nullptr) :Kula(0.0, 0.0, 0.0, 1.0),CAnnotation( m )
	{
		m_lats = m_longs = 128;
		setLabel("sphere");
	};

	CAnnotationSphere( CAnnotationSphere&t ) :Kula(t.center(),t.radius()), CAnnotation( t )
	{
		m_lats = m_longs = 128;
		setLabel("sphere");
	};

	CAnnotationSphere( Kula &k ) :Kula(k.center(), k.radius()), CAnnotation(nullptr)
	{
		m_lats = m_longs = 128;
		setLabel("sphere");
	};

	~CAnnotationSphere(void){};

	virtual int type() { return CAnnotation::SPHERE; }

	void applyTransform(CTransform prevT, CTransform newT=CTransform());

	virtual std::wstring getInfoRow();
	virtual std::wstring getTypeWSTR() { return L"sphere"; };

	void drawSphere(int lats, int longs);

	virtual void renderSelf() override;
};

