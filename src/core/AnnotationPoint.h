#pragma once
#include "Annotation.h"

#include "Point3.h"
#include "Vector3.h"
#include "RGBA.h"

class DPVISION_EXPORT CAnnotationPoint : public CAnnotation, public CPoint3d
{
	size_t m_faceId;
	CPoint3f m_faceA;
	CPoint3f m_faceB;
	CPoint3f m_faceC;

	CVector3d m_direction;

public:
	bool m_showNorm;
	CAnnotationPoint( std::shared_ptr<CBaseObject> m = nullptr) :CAnnotation( m ), CPoint3d()
	{
		setLabel("point");
		m_direction.Set( 0, 0, 0 );
		m_faceId = -1;
		m_showNorm = true;
	};

	CAnnotationPoint(int objId) :CAnnotation(objId), CPoint3d()
	{
		setLabel("point");
		m_direction.Set(0, 0, 0);
		m_faceId = -1;
		m_showNorm = true;
	};

	CAnnotationPoint( CAnnotationPoint &t ) :CAnnotation( t ), CPoint3d( t )
	{
		m_faceId = t.m_faceId;
		m_faceA = t.m_faceA;
		m_faceB = t.m_faceB;
		m_faceC = t.m_faceC;
		m_direction = t.m_direction;
		m_showNorm = true;
	};


	CAnnotationPoint( const double &tx, const double &ty, const double &tz ) :CAnnotation(-1), CPoint3d( tx, ty, tz )
	{
		//m_point.Set(tx, ty, tz);
		setLabel("point");
		m_direction.Set( 0, 0, 0 );
		m_faceId = -1;
		m_showNorm = true;
	};

	CAnnotationPoint( const CPoint3d &t ) :CAnnotation(-1), CPoint3d( t )
	{
		//m_point.Set( t );
		setLabel("point");
		m_direction.Set( 0, 0, 0 );
		m_faceId = -1;
		m_showNorm = true;
	};
	
	CAnnotationPoint( const CPoint3d &t, const CPoint3d &t0 ) :CAnnotation(-1), CPoint3d( t )
	{
		//m_point.Set(t);

		setLabel("point");
		m_direction = CVector3d(t0, t);
		m_direction.normalize();

		m_faceId = -1;
		m_showNorm = true;
	};

	CAnnotationPoint( const CPoint3d &t, const CRGBA kol ) :CAnnotation(-1), CPoint3d( t )
	{
		//m_point.Set( t );
		setLabel("point");
		m_direction.Set( 0, 0, 0 );
		m_faceId = -1;
		m_showNorm = true;
	};

	~CAnnotationPoint(void){};

	virtual std::shared_ptr<CBaseObject> getCopy() override
	{
		return std::make_shared<CAnnotationPoint>(*this);
	}

	virtual int type() { return CAnnotation::POINT; }

	virtual void applyTransformation(CTransform& from, CTransform& to) override;

	void setDirection(CVector3d ray) { m_direction.Set( ray ); };
	void setDirection(double x, double y, double z) { m_direction.Set(x,y,z); };

	CVector3d &getDirection() { return m_direction; };

	void setFace(size_t faceid, CPoint3f a, CPoint3f b, CPoint3f c) { m_faceId = faceid; m_faceA = a; m_faceB = b; m_faceC = c; };
	
	void setFaceId(size_t faceid) { m_faceId = faceid; };
	size_t getFaceId() { return m_faceId; };
	
	CPoint3f getFaceA() { return m_faceA; };
	CPoint3f getFaceB() { return m_faceB; };
	CPoint3f getFaceC() { return m_faceC; };

	virtual std::wstring getInfoRow();
	virtual std::wstring getTypeWSTR() { return L"point"; };

	virtual void renderSelf() override; 

	// OBSOLETE FUNCTIONS FOR BACK COMPATIBILITY

	// obsolete, use Set()
	void setPoint(const CPoint3d &t) { this->Set(t); };

	// obsolete, use Set()
	void setPoint(const double &tx, const double &ty, const double &tz) { this->Set(tx, ty, tz); };

	// obsolete
	CPoint3d getPoint() { return *this; };

	//obsolete, use setDirection()
	void setRay(CVector3d ray) { m_direction = ray; };

	//obsolete, use getDirection()
	CVector3d getRay() { return m_direction; };
};

