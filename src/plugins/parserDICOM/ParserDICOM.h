#pragma once

#include "Parser.h"

#include "imebra/imebra.h"
#include "Volumetric.h"
#include <exception>


class __declspec(dllexport) CParserDICOM : public CParser
{
	Volumetric::ColorSpace m_csp = Volumetric::ColorSpace::MONOCHROME2;

public:
	CParserDICOM(void);
	~CParserDICOM(void);

	//std::string decomposeFname(std::string fname, std::string& number);
	size_t parse_dicom_file(std::string dicomPath, uint16_t slide, Volumetric::SliceType& slice, Volumetric::SliceMetadata& metadata, bool isReversed);
	//void displayDicomTags(std::string fileName);
	long read_files(Volumetric *volum, int nbOfFiles, std::string dirPath, std::string shortName, std::string firstNumber, std::string suffix);
	long read_frames(Volumetric* volum, imebra::DataSet& dataSet, int nbOfFrames);
	int similarFilesExists(QString& first_file_path, QString& short_name, QString& last_file_path, QString& first_number, QString& last_number );
	//long readFromLastToFirst(Volumetric *volum, int nbOfFiles, std::string dirPath, std::string shortName, std::string firstNumber, std::string suffix);
	size_t Run();

	imebra::Image createImebraImageFromVoxels(const std::vector<Volumetric::VoxelType>& voxels, std::uint32_t width, std::uint32_t height);

	bool _export(Volumetric* volum, QString dir_name="", QString fname_base="slice");

	virtual bool save(CModel3D* obj, const QString path);
	virtual bool save(QVector<CBaseObject*> objects, const QString path);

};
