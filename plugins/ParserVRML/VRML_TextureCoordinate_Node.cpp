#include "StdAfx.h"

#include "ParserWRL.h"

void CParserWRL::VRML_ParseNode_TextureCoordinate( FILE *plik )
{
	char bufor[80];
	fscanf( plik, " %s [ ", bufor );
	if ( strstr( bufor, "point" ) )
	{
		VRML_ParseField_texpoint( plik );
	}
	
	do
	{
		fscanf( plik, " %s ", bufor );
	}
	while ( ! strstr( bufor, "}" ) );
}
