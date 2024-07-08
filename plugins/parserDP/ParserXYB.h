#pragma once
#include "Parser.h"

#define TYP_2D3F1D 0
#define TYP_3F 1

class CParserXYB :public CParser
{
public:
	CParserXYB(void);
	~CParserXYB(void);

	size_t Run();
};
