#include "StdAfx.h"

#include "ParserWRL.h"

void CParserWRL::VRML_ParseNode_Color( FILE *plik )
{
	char bufor[80];
	fscanf( plik, " %s [ ", bufor );
	if ( strstr( bufor, "color" ) )
	{
		long lc=0;
		float aR, aG, aB;

		UI::STATUSBAR::printf( "Reading Color section..." );

		while ( fscanf( plik, " %f %f %f", &aR, &aG, &aB ) == 3 )
		{
			pMeshData->vcolors().push_back( CRGBA( aR, aG, aB, 1.0 ) );
			
			lc++;
			fscanf( plik, " , " );
		}
		fscanf( plik, " ] " );
	}
	
	do
	{
		fscanf( plik, " %s ", bufor );
	}
	while ( ! strstr( bufor, "}" ) );
}
