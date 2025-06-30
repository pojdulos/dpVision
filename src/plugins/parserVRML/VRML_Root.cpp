#include "stdafx.h"

#include "ParserWRL.h"

void CParserWRL::VRML_Parse_Root( FILE *plik )
{
	char ch;
	char bufor[80];

	do
	{
		ch = VRML_SkipWhiteChars( plik );

		if ( ch == EOF )
		{
			return;
		}
		else if ( ch == '#' )
		{
			if ( ! VRML_GoToNextLine( plik ) )
			{
				fclose( plik );
				return;
			}
		}
		else if ( ch == 'T' )
		{
			fscanf( plik, " %s { ", bufor );
			if ( strstr( bufor, "ransform" ) )
			{
				VRML_ParseNode_Transform( plik );
			}
		}
		else if (ch == 'S')
		{
			fscanf(plik, " %s { ", bufor);
			if (strstr(bufor, "eparator"))
			{
				VRML_ParseNode_Separator(plik);
			}
		}
		else if ( ch == 'D' )
		{
			fscanf( plik, " %s ", bufor );
			if ( strstr( bufor, "EF" ) )
			{
				// Na razie ignoruj� definicje
				fscanf( plik, " %s ", bufor ); 
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
		else if ( ch == 'N' )
		{
			fscanf( plik, " %s { ", bufor );
			if ( strstr( bufor, "avigationInfo" ) )
			{
				VRML_IgnoreNode( plik, '{', '}' );
			}
		}
		else if ( ch == 'V' )
		{
			fscanf( plik, " %s { ", bufor );
			if ( strstr( bufor, "iewpoint" ) )
			{
				VRML_IgnoreNode( plik, '{', '}' );
			}
		}
//		else if ( ch == '{' )
//		{
//			VRML_IgnoreNode( plik, '{', '}' );
//		}
	}
	while ( ! feof( plik ) );
}
