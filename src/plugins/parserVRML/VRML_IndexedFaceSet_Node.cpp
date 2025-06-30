#include "stdafx.h"

#include "ParserWRL.h"

void CParserWRL::VRML_ParseNode_IndexedFaceSet( FILE *plik )
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
		    fscanf( plik, "%s ", bufor );
			if ( strstr( bufor, "oordIndex" ) )
			{
				VRML_ParseField_coordIndex( plik );
			}
			else if ( strstr( bufor, "oord" ) )
			{
				VRML_ParseField_coord( plik );
			}
			else if ( strstr( bufor, "olor" ) )
			{
				VRML_ParseField_color( plik );
			}
			else if ( strstr( bufor, "cw" ) )
			{
				fscanf( plik, "%s ", bufor );
			}
			else if ( strstr( bufor, "onvex" ) )
			{
				fscanf( plik, "%s ", bufor );
			}
			else if ( strstr( bufor, "reaseAngle" ) )
			{
				fscanf( plik, "%s ", bufor );
			}
		}
		else if ( ch == 's')
		{	
			fscanf( plik, "%s ", bufor );
			if ( strstr( bufor, "olid" ) )
			{
				fscanf( plik, "%s ", bufor );
			}
		}
		else if ( ch == 't')
		{	
		    fscanf( plik, "%s ", bufor );
			if ( strstr( bufor, "exCoordIndex" ) )
			{
				/* zawiera: [ 0 1 2 -1, 3 4 5 -1, 6 7 8 -1, 9 10 11 -1 ] */
				/* albo: [ 0, 1, 2, -1, 3, 4, 5, -1, 6, 7, 8, -1, 9, 10, 11, -1 ] */

				long a, b, c;
				long lti = 0;

				CTIndex indeks;

				UI::STATUSBAR::printf( "Reading TexCoordIndex section..." );

				fscanf( plik, " [ " );
	
				bool koniec = false;
				while ( ! koniec )
				{
					if ( 1 == fscanf( plik, " %d ", &a ) )
					{
						fscanf( plik, " , " ); // tu mo�e nie by� przecinka
						fscanf( plik, " %d ", &b );
						fscanf( plik, " , " ); // tu mo�e nie by� przecinka
						fscanf( plik, " %d ", &c );
						fscanf( plik, " , " ); // tu mo�e nie by� przecinka
						fscanf( plik, " -1 " );
						fscanf( plik, " , " ); // tu mo�e nie by� przecinka

						indeks.a = a;
						indeks.b = b;
						indeks.c = c;
			
						pMeshData->getMaterial().texindex.push_back( indeks );

						lti++;
					}
					else
					{
						char ch;
						fscanf( plik, " %c ", &ch );
						
						if ( ']' == ch )
						{
							koniec = true;
						}
					}
				}
			}
			else if ( strstr( bufor, "exCoord" ) )
			{
				fscanf( plik, " %s { ", bufor );
				if ( strstr( bufor, "TextureCoordinate" ) )
				{
					//VRML_IgnoreNode( plik, '{', '}' );
					VRML_ParseNode_TextureCoordinate( plik );

					/* zawiera:  point [ 0.1 0.2 0.3 0.4 0.5 ] 	*/
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
