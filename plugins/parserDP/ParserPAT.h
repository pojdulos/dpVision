#pragma once
#include "Parser.h"

#define TYP_2D3F1D 0
#define TYP_3F 1

class CPointCloud;

class CParserPAT :public CParser
{
	CMesh *m_mesh;

public:
	CParserPAT(void);
	~CParserPAT(void);

	//bool parseLine(char *bufor);

	size_t Run() override;

	bool save() override;
};
