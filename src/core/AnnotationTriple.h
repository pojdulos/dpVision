#pragma once
#include "Annotation.h"

#include "Point3.h"
#include "Vector3.h"
#include "RGBA.h"

class DPVISION_EXPORT CAnnotationTriple : public CAnnotation
{
public:
	CPoint3d m_origin;
	CVector3d m_rot;
	
	CPoint3d m_front;
	CPoint3d m_left;
	CPoint3d m_right;

	CAnnotationTriple(std::shared_ptr<CBaseObject> m = nullptr);;

	CAnnotationTriple( CAnnotationTriple &t );;

	~CAnnotationTriple(void){};

	CPoint3d &origin() { return m_origin; }
	CVector3d &rot() { return m_rot; }
	CPoint3d &front() { return m_front; }
	CPoint3d &left() { return m_left; }
	CPoint3d &right() { return m_right; }

	CPoint3d realPos(CPoint3d p);

	virtual int type() { return CAnnotation::TRIPLE; }

	void applyTransform(CTransform prevT, CTransform newT=CTransform());

	virtual std::wstring getInfoRow();
	virtual std::wstring getTypeWSTR() { return L"triple"; };

	void drawAxes();

	virtual void renderSelf() override;
};

