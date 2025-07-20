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

	CAnnotationBox(std::shared_ptr<CBaseObject> m = nullptr);

	CAnnotationBox( CAnnotationBox &p );

	~CAnnotationBox() {};

	virtual int type() { return CAnnotation::BOX; }

	virtual std::wstring getInfoRow();

};

