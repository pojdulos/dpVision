#pragma once
#include "Annotation.h"

#include "Punkt3D.h"
#include "Wektor3D.h"
#include "RGBA.h"
#include "BoundingBox.h"

#include <list>

class DPVISION_EXPORT CAnnotationBox : public CAnnotation, public CBoundingBox
{

public:
	//CPoint3f m_min;
	//CPoint3f m_max;

	CAnnotationBox( CBaseObject *m = nullptr) :CAnnotation(m)
	{
		setLabel("box");
	};

	CAnnotationBox( CAnnotationBox &p ) :CAnnotation(p)
	{
		setLabel("box");
		//m_min = p.m_min;
		//m_max = p.m_max;
	}

	~CAnnotationBox() {};

	virtual int type() { return CAnnotation::BOX; }

	virtual std::wstring getInfoRow();

	virtual void renderSelf() override;
};

