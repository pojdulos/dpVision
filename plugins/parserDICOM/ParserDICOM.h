#pragma once
#include "Parser.h"

#include "imebra/imebra.h"
#include "Volumetric.h"
#include <exception>

class __declspec(dllexport) CParserDICOM :public CParser
{
public:
	CParserDICOM(void);
	~CParserDICOM(void);

	std::string decomposeFname(std::string fname, std::string& number);
	size_t parse_dicom_file(std::string dicomPath, uint16_t slide, Volumetric::SliceDataType& slice, Volumetric::SliceMetadata& metadata, bool isReversed);
	//void displayDicomTags(std::string fileName);
	long read_files(Volumetric *volum, int nbOfFiles, std::string dirPath, std::string shortName, std::string firstNumber, std::string suffix);
	//long readFromLastToFirst(Volumetric *volum, int nbOfFiles, std::string dirPath, std::string shortName, std::string firstNumber, std::string suffix);
	size_t Run();

	imebra::Image createImebraImageFromVoxels(const std::vector<float>& voxels, std::uint32_t width, std::uint32_t height);

	bool _export(Volumetric* volum, QString dir_name="v:/test/", QString fname_base="plaster");

	virtual bool save(CModel3D* obj, const QString path);
	virtual bool save(QVector<CBaseObject*> objects, const QString path);

};
