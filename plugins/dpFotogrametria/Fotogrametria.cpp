#include "stdafx.h"
#include "Fotogrametria.h"

#include "AP.h"

#include "AnnotationCamera.h"

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"
#include "rapidxml/rapidxml_utils.hpp"

using namespace rapidxml;

void Fotogrametria::loadAgisoftCamerasXML(std::string path)
{
	static double matrix[16];

	xml_document<char> doc;

	rapidxml::file<> file(path.c_str());

	doc.parse<0>(file.data());

	rapidxml::xml_node<>* root = doc.first_node("document");

	xml_node<>* chunk;

	while (chunk = root->first_node("chunk"))
	{
		std::string chunkLabel = chunk->first_attribute("label")->value();

		CModel3D* target = AP::WORKSPACE::getCurrentModel();
		if (target == nullptr)
		{
			target = new CModel3D();
			target->setLabel(UI::str2wstr(chunkLabel));
			AP::WORKSPACE::addModel(target);
		}


		xml_node<>* sensors = chunk->first_node("sensors");
		int sensorsCount = std::stoi(sensors->first_attribute("next_id")->value());

		std::vector<CAnnotationCamera::Sensor> sensorInfo(sensorsCount);

		xml_node<>* sensor;
		while (sensor = sensors->first_node("sensor"))
		{
			int sensorId = std::stoi(sensor->first_attribute("id")->value());

			xml_node<>* resolution = sensor->first_node("resolution");
			sensorInfo[sensorId].xResolution = std::stoi(resolution->first_attribute("width")->value());
			sensorInfo[sensorId].yResolution = std::stoi(resolution->first_attribute("height")->value());

			sensorInfo[sensorId].mmFocalLength = 50.0;
			
			double pixWidth = 0.00376;
			double pixHeight = 0.00376;

			xml_node<>* property;
			while (property = sensor->first_node("property"))
			{
				std::string name = property->first_attribute("name")->value();
				if (name == "pixel_width")
				{
					pixWidth = std::stod(property->first_attribute("value")->value());
				}
				else if (name == "pixel_height")
				{
					pixHeight = std::stod(property->first_attribute("value")->value());
				}
				else if (name == "focal_length")
				{
					sensorInfo[sensorId].mmFocalLength = std::stod(property->first_attribute("value")->value());
				}
				sensor->remove_node(property);
			}

			sensorInfo[sensorId].setPixelSize(pixWidth, pixHeight);

			xml_node<>* calibration = sensor->first_node("calibration");

			if (calibration != nullptr)
				sensorInfo[sensorId].pxFocalLenght = std::stod(calibration->first_node("f")->value());

			sensors->remove_node(sensor);
		}




		xml_node<>* cameras = chunk->first_node("cameras");

		xml_node<>* camera;
		xml_attribute<>* attr;
		xml_node<>* node;
		std::string matrixSTR("1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1");

		std::string matrixS[16];

		CAnnotation* setOfCam = new CAnnotation();
		setOfCam->setLabel(UI::str2wstr(chunkLabel));
		target->addAnnotation(setOfCam);

		while (camera = cameras->first_node("camera"))
		{
			memchr(matrix, 0, 16 * sizeof(double));

			std::string id = camera->first_attribute("id")->value();
			std::string cameraLabel = camera->first_attribute("label")->value();

			int sensorId = std::stoi(camera->first_attribute("sensor_id")->value());

			xml_node<>* transform = camera->first_node("transform");

			if (transform)
			{
				matrixSTR = transform->value();
			}

			CAnnotationCamera* an = CAnnotationCamera::create( sensorInfo[sensorId], matrixSTR, cameraLabel );
			
			setOfCam->addAnnotation(an);

			cameras->remove_node(camera);
		}


		root->remove_node(chunk);

	}

	UI::DOCK::WORKSPACE::rebuildTree();
}



void Fotogrametria::addCameraToModel( CModel3D *m_model, double pixelWidth, double pixelHeight, int width, int height, double focalLenght, double matrix[16], std::string label)
{
	CAnnotationCamera* ad = new CAnnotationCamera();

	ad->sensor().setPixelSize(pixelWidth, pixelHeight);
	ad->sensor().setResolution(width, height);
	ad->sensor().setFocalLenght(focalLenght);
	ad->setMatrix(matrix);

	ad->setLabel(UI::str2wstr(label));

	ad->setColor(CRGBA::fromHexRGBA("#007bff66"));

	m_model->addAnnotation(ad);

}
