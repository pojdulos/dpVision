#pragma once
#include "Parser.h"

struct node
{
	std::string name;

};

struct knownNodes
{

};

class CParserWRL : public CParser
{
	int m_vrmlVersion;

	bool VRML_IgnoreNode( FILE *plik, char ch1, char ch2 );
	
	char VRML_SkipWhiteChars( FILE *plik );

	void VRML_Parse_Root( FILE *plik );

	void VRML_ParseNode_Transform( FILE *plik );
	void VRML_ParseNode_Separator(FILE *plik);

	void VRML_ParseNode_Shape( FILE *plik );

	void VRML_ParseField_children( FILE *plik );

	void VRML_ParseNode_IndexedFaceSet( FILE *plik );
	void VRML_ParseNode_Appearance( FILE *plik );
	void VRML_ParseNode_Coordinate( FILE *plik );

	size_t VRML_ParseField_texpoint( FILE *plik );
	void VRML_ParseNode_TextureCoordinate( FILE *plik );

	void VRML_ParseNode_Color( FILE *plik );
	void VRML_ParseNode_Material(FILE *plik);

	void VRML_ParseField_coord( FILE *plik );
	size_t VRML_ParseField_point( FILE *plik );
	size_t VRML_ParseField_coordIndex( FILE *plik );
	void VRML_ParseField_color( FILE *plik );
	bool VRML_GoToNextLine( FILE *plik );
	bool VRML_FindChar( FILE *plik, char znak );

	size_t ReadVRML();

public:
	CParserWRL(void);
	~CParserWRL(void);

	size_t Run();
};
