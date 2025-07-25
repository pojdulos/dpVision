#pragma once
#include "Annotation.h"
#include "Plane.h"

#include "Punkt3D.h"
#include "Wektor3D.h"
#include "RGBA.h"

//class CTransform;

class DPVISION_EXPORT CAnnotationPlane : public CAnnotation, public CPlane
{
	double m_size;

public:
	CAnnotationPlane(std::shared_ptr<CBaseObject> m = nullptr);
	CAnnotationPlane(int objId);
	CAnnotationPlane( CAnnotationPlane &t );
	CAnnotationPlane( CPlane p );
	CAnnotationPlane(CPoint3d p, CVector3d n);

	~CAnnotationPlane(void) override {};

	virtual int type() { return CAnnotation::PLANE; }

	virtual std::shared_ptr<CBaseObject> getCopy() override
	{
		auto obj = std::make_shared<CAnnotationPlane>(*this);
		updateChildrenParentPointers(obj);
		return obj;
	}

	void setSize(double s) { m_size = s; };
	double getSize() { return m_size; };

	virtual void applyTransformation(CTransform &prevT, CTransform &newT) override;

	virtual std::wstring getInfoRow() override;
	virtual std::wstring getTypeWSTR() override { return L"plane"; };
};

