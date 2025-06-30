#include "stdafx.h"

#include "ParserWRL.h"

void CParserWRL::VRML_ParseField_color( FILE *plik )
{
	char bufor[80];
	fscanf( plik, " %s { ", bufor );
	if ( strstr( bufor, "Color" ) )
	{
		VRML_ParseNode_Color( plik );
	}
}
