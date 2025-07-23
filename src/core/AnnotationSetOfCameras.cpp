#include "Global.h"
#include "AnnotationSetOfCameras.h"
#include "AnnotationCamera.h"
#include "Model3D.h"

#include "../api/AP.h"

#include <regex>

#include "../renderers/IAnnotationSetOfCamerasRenderer.h"

CAnnotationSetOfCameras::CAnnotationSetOfCameras(std::shared_ptr<CBaseObject> m) :CAnnotation(m)
{
	setLabel("set of Cameras");
	renderer_ = std::make_shared<IAnnotationSetOfCamerasRenderer>();
}

CAnnotationSetOfCameras::CAnnotationSetOfCameras(CAnnotationSetOfCameras& p) : CAnnotation(p)
{
	setLabel("set of Cameras");
	renderer_ = std::make_shared<IAnnotationSetOfCamerasRenderer>();
}

std::wstring CAnnotationSetOfCameras::getInfoRow()
{
	std::wstring info = L"Set of Cameras";

	return info;
}
