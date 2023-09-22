#pragma once
#include "Parser.h"
#include <sstream>

typedef std::map<int, int> _mapa;

class CParserSMF :public CParser
{
public:
	CParserSMF(void);
	~CParserSMF(void);

	bool inPlugin() { return false; };

	size_t Run();
private:
	size_t ReadSMF_V3();
	size_t ReadSMF_V3b();
	void SkipLine( FILE *plik );
	void ReadVSplit( FILE *plik, SVsplit & vsplit );
	void ReadPartFromV4( FILE *plik, SVsplit & vsplit );
	void ReadPartFromV5( FILE *plik, SVsplit & vsplit );
};
