#include "StdAfx.h"

#include "ParserWRL.h"

void CParserWRL::VRML_ParseField_coord( FILE *plik )
{
	char bufor[80];
	char ch;
	fscanf( plik, " %s { ", bufor );
	if ( strstr( bufor, "Coordinate" ) )
	{
		VRML_ParseNode_Coordinate( plik );
	}
	else if ( strstr( bufor, "DEF" ) ) // DEF C Coordinate
	{
		ch = fgetc(plik);
		fscanf( plik, " %s { ", bufor );
		if ( strstr( bufor, "Coordinate" ) )
			VRML_ParseNode_Coordinate( plik );
	}
}
