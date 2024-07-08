#pragma once
#include "Parser.h"

#define TYP_2D3F1D 0
#define TYP_3F 1

class CModel3D;
class CPointCloud;

class CParserPLY :public CParser
{
	typedef enum {
		NoColors,
		UcharRGB,
		UcharRGBA
	} ColorFormat;

	typedef enum {
		Ascii,
		BinLE,
		BinBE
	} FileFormat;

	FileFormat fileFormat; //0 = ascii 1.0, 1 = binary_little_endian 1.0
	size_t verticesToRead;
	size_t facesToRead;

	ColorFormat colorFormat;
	
	uchar propertyIsFor; //0=undefined, 1=vertex, 2=face
	void readVerticesBIN(QFile& inputFile, CPointCloud* destPC);
	void readVerticesTXT(QFile& inputFile, CPointCloud* destPC);
	void readFacesBIN(QFile& inputFile, CMesh* destM);
	void readFacesTXT(QFile& inputFile, CMesh* destM);

	void parseVproperty(QString line);
	void parseFproperty(QString line);

public:
	CParserPLY(void);
	~CParserPLY(void);

	bool parseHeader(QString line);

	bool parseVertexTXT(QString line, CVertex& v, CRGBA& c);

	bool parseFaceTXT(QString line, CFace& face);

	virtual CModel3D* load(const QString path, bool wait = false) override;
	virtual bool save() override;
};
