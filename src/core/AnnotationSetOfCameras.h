#pragma once
#include "Annotation.h"

#include "Punkt3D.h"
#include "Wektor3D.h"
#include "RGBA.h"
#include "Transform.h"

class DPVISION_EXPORT CAnnotationSetOfCameras : public CAnnotation
{
public:
	CAnnotationSetOfCameras(std::shared_ptr<CBaseObject> m = nullptr);;

	CAnnotationSetOfCameras( CAnnotationSetOfCameras &p );

	virtual int type() { return CAnnotation::SETOFCAMERAS; }

	virtual std::wstring getInfoRow();
};

