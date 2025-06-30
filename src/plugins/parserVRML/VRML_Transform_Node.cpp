#include "stdafx.h"

#include "ParserWRL.h"

void CParserWRL::VRML_ParseNode_Transform( FILE *plik )
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
		else if ( ch == 'c')
		{	
		    fscanf( plik, "%s [ ", bufor );
			if ( strstr( bufor, "hildren" ) )
			{
				VRML_ParseField_children( plik );
			}
		}
		else if ( ch == 't')
		{	
			// translation 0.000000 0.000000 0.000000
			// na razie nie obs�uguj�
		    if( ! VRML_GoToNextLine( plik ) ) return;
		}
		else if ( ch == 'r')
		{	
			// rotation	0.000000 0.000000 1.000000 0.000000
			// na razie nie obs�uguj�
		    if( ! VRML_GoToNextLine( plik ) ) return;
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
