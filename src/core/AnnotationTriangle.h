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

	CAnnotationTriangle(std::shared_ptr<CBaseObject> m = nullptr);;

	CAnnotationTriangle( CAnnotationTriangle &t );;

	CAnnotationTriangle(CTriangle& t);;

	CAnnotationTriangle(CPoint3d A, CPoint3d B, CPoint3d C);;

	CAnnotationTriangle(CPoint3d v[3]);;

	~CAnnotationTriangle(void){};

	virtual std::shared_ptr<CBaseObject> getCopy() override
	{
		auto obj = std::make_shared<CAnnotationTriangle>(*this);
		updateChildrenParentPointers(obj);
		return obj;

	}

	CPoint3d &C() { return m_pC; }
	CPoint3d &A() { return m_pA; }
	CPoint3d &B() { return m_pB; }

	CTriangle toTriangle() { return CTriangle(m_pA, m_pB, m_pC); };

	virtual int type() { return CAnnotation::TRIPLE; };

	void applyTransform(CTransform prevT, CTransform newT=CTransform());

	virtual std::wstring getInfoRow();
	virtual std::wstring getTypeWSTR() { return L"triangle"; };

};

