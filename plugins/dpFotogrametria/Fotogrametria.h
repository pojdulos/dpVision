#pragma once

#include "Workspace.h"

class  __declspec(dllexport) Fotogrametria
{
public:
	static void loadAgisoftCamerasXML(std::string path);

	static void addCameraToModel(CModel3D* m_model, double pixelWidth, double pixelHeight, int width, int height, double focalLenght, double matrix[16], std::string label="");

};

