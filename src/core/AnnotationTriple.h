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

	CAnnotationTriple( CBaseObject *m = nullptr) :CAnnotation( m )
	{
		m_origin = CPoint3d(0.0,0.0,0.0);
		m_rot = CVector3d(0.0,0.0,0.0);

		setLabel("triple");
	};

	CAnnotationTriple( CAnnotationTriple &t ) :CAnnotation( t )
	{
		m_origin = t.m_origin;
		m_rot = t.m_rot;

		setLabel("triple");
		m_front = t.m_front;
		m_left = t.m_left;
		m_right = t.m_right;
	};

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

