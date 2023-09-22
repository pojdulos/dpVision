#pragma once
#include "Annotation.h"

#include "Punkt3D.h"
#include "Wektor3D.h"
#include "RGBA.h"
#include "Transform.h"

class __declspec(dllexport)  CAnnotationSetOfCameras : public CAnnotation
{
public:
	CAnnotationSetOfCameras( CBaseObject *m = nullptr) :CAnnotation(m)
	{
		setLabel("set of Cameras");
	};

	CAnnotationSetOfCameras( CAnnotationSetOfCameras &p ) :CAnnotation(p)
	{
		setLabel("set of Cameras");
	}

	virtual int type() { return CAnnotation::SETOFCAMERAS; }

	virtual std::wstring getInfoRow();
};

