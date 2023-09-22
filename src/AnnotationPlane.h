#pragma once
#include "Annotation.h"
#include "Plane.h"

#include "Punkt3D.h"
#include "Wektor3D.h"
#include "RGBA.h"

//class CTransform;

class __declspec(dllexport)  CAnnotationPlane : public CAnnotation, public CPlane
{
	double m_size;

public:
	CAnnotationPlane( CBaseObject *m = nullptr) :CAnnotation( m ),CPlane()
	{
		setLabel("plane");
		m_color.SetFloat(0.0f, 1.0f, 1.0f, 0.5f);
		m_selcolor.SetFloat(1.0f, 0.0f, 0.0f, 0.5f);
		m_size = 10;
	};

	CAnnotationPlane(int objId) :CAnnotation(objId), CPlane()
	{
		setLabel("plane");
		m_color.SetFloat(0.0f, 1.0f, 1.0f, 0.5f);
		m_selcolor.SetFloat(1.0f, 0.0f, 0.0f, 0.5f);
		m_size = 10;
	};

	CAnnotationPlane( CAnnotationPlane &t ) :CAnnotation( t ),CPlane( t )
	{
		setLabel("plane");
		m_color.SetFloat(0.0f, 1.0f, 1.0f, 0.5f);
		m_selcolor.SetFloat(1.0f, 0.0f, 0.0f, 0.5f);
		m_size = 10;
	};


	CAnnotationPlane( CPlane p ) :CAnnotation(-1), CPlane(p)
	{
		setLabel("plane");
		m_normal.normalize();
		m_color.SetFloat(0.0f, 1.0f, 1.0f, 0.5f);
		m_selcolor.SetFloat(1.0f, 0.0f, 0.0f, 0.5f);
		m_size = 10;
	};

	CAnnotationPlane(CPoint3d p, CVector3d n) :CAnnotation(-1), CPlane(p, n)
	{
		setLabel("plane");
		m_normal.normalize();
		m_color.SetFloat(0.0f, 1.0f, 1.0f, 0.5f);
		m_selcolor.SetFloat(1.0f, 0.0f, 0.0f, 0.5f);
		m_size = 10;
	};


	~CAnnotationPlane(void) override {};

	virtual int type() { return CAnnotation::PLANE; }

	void setSize(double s) { m_size = s; };
	double getSize() { return m_size; };

	virtual void applyTransformation(CTransform &prevT, CTransform &newT) override;

	virtual std::wstring getInfoRow() override;
	virtual std::wstring getTypeWSTR() override { return L"plane"; };

	virtual void renderSelf() override;

};

