#pragma once
#include "Parser.h"

#define TYP_2D3F1D 0
#define TYP_3F 1

class CPointCloud;

class CParserPTS :public CParser
{
	CPointCloud *m_cloud;

public:
	CParserPTS(void);
	~CParserPTS(void);

	bool parseLine(char *bufor);

	size_t Run() override;

	bool save() override;
};
