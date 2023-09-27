#include "StdAfx.h"

#include "ParserWRL.h"

void CParserWRL::VRML_ParseNode_Shape( FILE *plik )
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
		else if ( ch == 'a')
		{	
		    fscanf( plik, "%s ", bufor );
			if ( strstr( bufor, "ppearance" ) )
			{
				fscanf( plik, " %s { ", bufor );
				if ( strstr( bufor, "Appearance" ) )
				{
					VRML_ParseNode_Appearance( plik );
					//VRML_IgnoreNode( plik, '{', '}' );
				}
			}
		}
		else if ( ch == 'g')
		{	
		    fscanf( plik, "%s ", bufor );
			if ( strstr( bufor, "eometry" ) )
			{
				fscanf( plik, " %s { ", bufor );
				if ( strstr( bufor, "IndexedFaceSet" ) )
				{
					VRML_ParseNode_IndexedFaceSet( plik );
				}
				else if ( strstr( bufor, "PointSet" ) ) 
				{
					VRML_ParseNode_IndexedFaceSet( plik );
				}
			}
		}
		else if ( ch == '{' )
		{
			VRML_IgnoreNode( plik, '{', '}' );
		}
		else if ( ch == '}')
		{
			return;
		}
	}
	while ( ! feof( plik ) ); 
}
