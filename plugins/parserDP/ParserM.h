#pragma once
#include "Parser.h"

#define TYP_2D3F1D 0
#define TYP_3F 1

class CPointCloud;

class CParserM :public CParser
{
	CPointCloud *m_cloud;

public:
	CParserM(void);
	~CParserM(void);

	bool parseLine(char *bufor);

	size_t Run() override;

	bool save() override;
};
