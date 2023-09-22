#pragma once
#include "Annotation.h"

#include "Punkt3D.h"
#include "Wektor3D.h"
#include "RGBA.h"

#include <list>

class __declspec(dllexport)  CAnnotationPyramid : public CAnnotation
{
	CPoint3d m_bottom[4];
	CPoint3d m_top[4];

public:
	CAnnotationPyramid( CBaseObject *m = nullptr) :CAnnotation(m)
	{
		setLabel("Pyramid");
	};

	CAnnotationPyramid( CAnnotationPyramid &p ) :CAnnotation(p)
	{
		setLabel("Pyramid");
		m_top[0] = p.m_top[0];
		m_top[1] = p.m_top[1];
		m_top[2] = p.m_top[2];
		m_top[3] = p.m_top[3];
		m_bottom[0] = p.m_bottom[0];
		m_bottom[1] = p.m_bottom[1];
		m_bottom[2] = p.m_bottom[2];
		m_bottom[3] = p.m_bottom[3];
	}

	~CAnnotationPyramid() {};

	virtual int type() { return CAnnotation::PYRAMID; }

	void setBottom(const CPoint3d &b0, const CPoint3d &b1, const CPoint3d &b2, const CPoint3d &b3)
	{
		m_bottom[0] = b0;
		m_bottom[1] = b1;
		m_bottom[2] = b2;
		m_bottom[3] = b3;
	};

	void setTop(const CPoint3d &t0, const CPoint3d &t1, const CPoint3d &t2, const CPoint3d &t3)
	{
		m_top[0] = t0;
		m_top[1] = t1;
		m_top[2] = t2;
		m_top[3] = t3;
	};

	void setTop(const CPoint3d &t0)
	{
		m_top[0] = t0;
		m_top[1] = t0;
		m_top[2] = t0;
		m_top[3] = t0;
	};

	virtual std::wstring getInfoRow();

	virtual void renderSelf() override;
};

