#include "StdAfx.h"

#include "ParserWRL.h"

void CParserWRL::VRML_ParseNode_Appearance( FILE *plik )
{
	char bufor[80];
	char ch; 
	do
	{
		do { ch = fgetc( plik ); } while ( ch == ' ' );
		//ch = fgetc(plik); 
		if ( ch == '#')
		{
			if( ! VRML_GoToNextLine( plik ) ) return;
		}
		else if ( ch == 'm')
		{	
		    fscanf( plik, "%s ", bufor );
			if ( strstr( bufor, "aterial" ) )
			{
				fscanf( plik, " %s { ", bufor );
				if ( strstr( bufor, "Material" ) )
				{
					VRML_ParseNode_Material(plik);
				}
				else
				  VRML_IgnoreNode( plik, '{', '}' );
			}
		}
		else if ( ch == 't')
		{	
		    fscanf( plik, "%s ", bufor );
			if ( strstr( bufor, "exture" ) )
			{
				fscanf( plik, " %s { ", bufor );
				if ( strstr( bufor, "ImageTexture" ) )
				{
					fscanf( plik, " url ", bufor );

					// uwaga - nawias prostok¹tny czasem jest a czasem go nie ma !!!
					fscanf( plik, " [ ", bufor );
					
					// cudzys³ów raczej zawsze jest
					fscanf( plik, " \" ", bufor );
					
					int i = -1;
					do
					{
						i++;
						bufor[i] = fgetc( plik );			
					}
					while ( bufor[i] != '"' );

					bufor[i] = '\0';
					
					pMeshData->getMaterial().TexInfo = QString( bufor );

					// uwaga - nawias prostok¹tny czasem jest a czasem go nie ma !!!
					fscanf( plik, " ] ", bufor );
					VRML_IgnoreNode( plik, '{', '}' );
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
