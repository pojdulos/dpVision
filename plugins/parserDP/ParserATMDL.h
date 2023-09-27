#pragma once
#include "Parser.h"

#define TYP_2D3F1D 0
#define TYP_3F 1

class CPointCloud;

class CParserATMDL :public CParser
{
public:
	CParserATMDL(void);
	~CParserATMDL(void);

	CModel3D* load(const QString path, bool wait) override;
	//bool save(CModel3D* obj, const QString path) override;
	//bool save() override;
};
