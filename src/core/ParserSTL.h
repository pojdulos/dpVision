#pragma once
#include "Parser.h"
#include "Global.h"
#include <map>

struct SStlFace {
	CVector3f n;
	CVertex a;
	CVertex b;
	CVertex c;
};

struct SVertex {
	CVertex v;
	size_t iId;
};

struct ltCVertex
{
  bool operator()( CVertex p1, CVertex p2 ) const
  {
		if ( p1.X() < p2.X() ) return true;
		else if ( p1.X() > p2.X() ) return false;
		else if ( p1.Y() < p2.Y() ) return true;
		else if ( p1.Y() > p2.Y() ) return false;
		else if ( p1.Z() < p2.Z() ) return true;
		return false;
  }
};

class CParserSTL : public CParser {
	std::map< CVertex, size_t, ltCVertex > mMapa;
	size_t ReadTextSTL();
	size_t ReadBinarySTL();
public:
	CParserSTL(void);
	~CParserSTL(void);
	size_t Run();
	bool inPlugin() { return false; };
};
