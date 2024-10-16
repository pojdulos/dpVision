#include "StdAfx.h"

#include "ParserWRL.h"

void CParserWRL::VRML_ParseField_children( FILE *plik )
{
	char bufor[80];
	char ch; 
	do
	{
		do { ch = fgetc( plik ); } while ( ch == ' ' ); 
		if ( ch == '#')
		{
			if( ! VRML_GoToNextLine( plik ) ) return;
		}
		else if ( ch == 'T' )
		{
			fscanf( plik, " %s { ", bufor );
			if ( strstr( bufor, "ransform" ) )
			{
				VRML_ParseNode_Transform( plik );
			}
		}
		else if ( ch == 'S' )
		{
			fscanf( plik, " %s { ", bufor );
			if ( strstr( bufor, "hape" ) )
			{
				VRML_ParseNode_Shape( plik );
			}
		}
		else if ( ch == ']')
		{
			return;
		}
	}
	while ( ! feof( plik ) ); 
}
