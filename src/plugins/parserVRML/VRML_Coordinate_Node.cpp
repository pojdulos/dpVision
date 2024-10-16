#include "StdAfx.h"

#include "ParserWRL.h"

void CParserWRL::VRML_ParseNode_Coordinate( FILE *plik )
{
	char bufor[80];
	fscanf( plik, " %s [ ", bufor );
	if ( strstr( bufor, "point" ) )
	{
		VRML_ParseField_point( plik );
	}
	
	do
	{
		fscanf( plik, " %s ", bufor );
	}
	while ( ! strstr( bufor, "}" ) );
}
