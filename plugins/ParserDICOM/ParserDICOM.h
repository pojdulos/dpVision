#pragma once
#include "Parser.h"

#define TYP_2D3F1D 0
#define TYP_3F 1

class CVolTK;
class RAWdicomData;

#include <string>

#include "imebra/imebra.h"
#include <exception>

class __declspec(dllexport) CParserDICOM :public CParser
{
	

	struct {
		std::string photometric;
		uint32_t height;
		uint32_t width;
		uint32_t depth;
		CVector3d firstImagePosition;
		CVector3d lastImagePosition;
		int pixelRepresentation;
		int32_t rescaleIntercept;
		int32_t rescaleSlope;
		int rescaleType;
		int32_t windowCenter;
		int32_t windowWidth;
	} m_tags;

public:
	CParserDICOM(void);
	~CParserDICOM(void);

	std::string decomposeFname(std::string fname, std::string& number);
	size_t parseSingleFile(std::string currentDir, std::string shortName, std::string currentNumber, std::string extension);
	size_t parseSingleDICOM(std::string dicomPath, std::string currentNumber);
	size_t parseSingleDICOM2volTK(std::string dicomPath, uint16_t slide, RAWdicomData& kostka, double& sliceLocation, bool isReversed=false);
	void displayDicomTags(std::string fileName);
	long readFromFirstToLast(RAWdicomData& kostka, int nbOfFiles, std::string dirPath, std::string shortName, std::string firstNumber, std::string suffix);
	long readFromLastToFirst(RAWdicomData& kostka, int nbOfFiles, std::string dirPath, std::string shortName, std::string firstNumber, std::string suffix);
	size_t Run();
};
