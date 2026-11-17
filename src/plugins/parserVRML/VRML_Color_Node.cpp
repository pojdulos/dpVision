#include "stdafx.h"

#include "ParserWRL.h"

#include "../api/adapters/PluginHostAPIAdapter.h"

namespace {
PluginHostAPIAdapter& hostApi()
{
    static PluginHostAPIAdapter api;
    return api;
}
}

void CParserWRL::VRML_ParseNode_Color( FILE *plik )
{
	char bufor[80];
	fscanf( plik, " %s [ ", bufor );
	if ( strstr( bufor, "color" ) )
	{
		long lc=0;
		float aR, aG, aB;

		hostApi().statusBar().setText("Reading Color section...");

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
