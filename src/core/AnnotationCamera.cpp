#include "Global.h"
#include "AnnotationCamera.h"
#include "Model3D.h"

#include "../api/AP.h"


#include <regex>

#include "../renderers/IAnnotationCameraRenderer.h"
#include "utils/StringUtils.h"

CAnnotationCamera::CAnnotationCamera(std::shared_ptr<CBaseObject> m) :CAnnotation(m)
{
	setLabel("Camera");

	renderer_ = std::make_shared<IAnnotationCameraRenderer>();
}

CAnnotationCamera::CAnnotationCamera(CAnnotationCamera& p) : CAnnotation(p)
{
	setLabel("Camera");

	renderer_ = std::make_shared<IAnnotationCameraRenderer>();
}

std::shared_ptr<CAnnotationCamera> CAnnotationCamera::create(Sensor s, double matrix[16], std::string label)
{
	auto ad = std::make_shared<CAnnotationCamera>();

	ad->m_sensor = s;
	
	ad->setMatrix(matrix);

	ad->setLabel(dpVisionCore::utils::str2wstr(label));

	//ad->setColor(CRGBA::fromHexRGBA("#007bff66"));

	return ad;
}

std::shared_ptr<CAnnotationCamera> CAnnotationCamera::create(Sensor s, std::string matrixSTR, std::string label)
{
	auto ad = std::make_shared<CAnnotationCamera>();

	ad->m_sensor = s;

	ad->setMatrix(matrixSTR);

	ad->setLabel(dpVisionCore::utils::str2wstr(label));

	ad->setColor(CRGBA::fromHexRGBA("#007bff66"));
	ad->setSelColor(CRGBA::fromHexRGBA("#ff000066"));

	return ad;
}


std::vector<std::string> resplit(const std::string& s, std::string rgx_str = "\\s+")
{
	std::vector<std::string> elems;

	std::regex rgx(rgx_str);

	std::sregex_token_iterator iter(s.begin(), s.end(), rgx, -1);
	std::sregex_token_iterator end;

	while (iter != end) {
		//std::cout << "S43:" << *iter << std::endl;
		elems.push_back(*iter);
		++iter;
	}

	return elems;

}

void CAnnotationCamera::setMatrix(std::string matrixSTR)
{
	std::regex rgx("\\s+");

	std::sregex_token_iterator iter(matrixSTR.begin(), matrixSTR.end(), rgx, -1);
	std::sregex_token_iterator end;

	int row = 0;
	int col = 0;
	while (iter != end) {
		matrix(col, row) = std::stod(*iter);
		++iter;

		col++;
		if (col > 3)
		{
			row++;
			col = 0;
		}
	}
}


void CAnnotationCamera::setMatrix(double matrix[16])
{
	m_matrix[0] = matrix[0];
	m_matrix[1] = matrix[4];
	m_matrix[2] = matrix[8];
	m_matrix[3] = matrix[12];
	m_matrix[4] = matrix[1];
	m_matrix[5] = matrix[5];
	m_matrix[6] = matrix[9];
	m_matrix[7] = matrix[13];
	m_matrix[8] = matrix[2];
	m_matrix[9] = matrix[6];
	m_matrix[10] = matrix[10];
	m_matrix[11] = matrix[14];
	m_matrix[12] = matrix[3];
	m_matrix[13] = matrix[7];
	m_matrix[14] = matrix[11];
	m_matrix[15] = matrix[15];

	//for (int i = 0; i < 16; i++)
	//{
	//	m_matrix[i] = matrix[i];
	//}
}

std::wstring CAnnotationCamera::getInfoRow()
{
	std::wstring info = L"Camera";

	return info;
}

