#pragma once
#include "Annotation.h"
#include "Triangle.h"

#include "Point3.h"
#include "Vector3.h"
#include "RGBA.h"

class DPVISION_EXPORT CAnnotationTriangle : public CAnnotation
{
public:
	CPoint3d m_pA, m_pB, m_pC;

	CAnnotationTriangle( CBaseObject *m = nullptr) :CAnnotation( m )
	{
		setLabel("triple");
	};

	CAnnotationTriangle( CAnnotationTriangle &t ) :CAnnotation( t )
	{
		setLabel("triple");
		m_pC = t.m_pC;
		m_pA = t.m_pA;
		m_pB = t.m_pB;
	};

	CAnnotationTriangle(CTriangle& t) :CAnnotation(nullptr)
	{
		setLabel("triple");
		m_pC = t.m_pV[2];
		m_pA = t.m_pV[0];
		m_pB = t.m_pV[1];
	};

	CAnnotationTriangle(CPoint3d A, CPoint3d B, CPoint3d C) :CAnnotation(nullptr)
	{
		setLabel("triple");
		m_pA = A;
		m_pB = B;
		m_pC = C;
	};

	CAnnotationTriangle(CPoint3d v[3]) :CAnnotation(nullptr)
	{
		setLabel("triple");
		m_pA = v[0];
		m_pB = v[1];
		m_pC = v[2];
	};

	~CAnnotationTriangle(void){};

	CPoint3d &C() { return m_pC; }
	CPoint3d &A() { return m_pA; }
	CPoint3d &B() { return m_pB; }

	CTriangle toTriangle() { return CTriangle(m_pA, m_pB, m_pC); };

	virtual int type() { return CAnnotation::TRIPLE; };

	void applyTransform(CTransform prevT, CTransform newT=CTransform());

	virtual std::wstring getInfoRow();
	virtual std::wstring getTypeWSTR() { return L"triangle"; };

	virtual void renderSelf() override;
};

