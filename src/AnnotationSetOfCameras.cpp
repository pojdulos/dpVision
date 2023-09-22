#include "Global.h"
#include "AnnotationSetOfCameras.h"
#include "AnnotationCamera.h"
#include "Model3D.h"

#include "AP.h"
#include "UI.h"

#include <Windows.h>
#include <GL/GL.h>

#include <regex>

std::wstring CAnnotationSetOfCameras::getInfoRow()
{
	std::wstring info = L"Set of Cameras";

	return info;
}
