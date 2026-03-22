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

size_t CParserWRL::VRML_ParseField_texpoint( FILE *plik )
{
	float s, t;
	size_t ltp = 0;

	CTCoord koord;

	hostApi().statusBar().setText("Reading TextPoint section...");

	while ( fscanf( plik, " %f %f ", &s, &t ) == 2)
	{
		koord[0] = s;
		koord[1] = t;
		pMeshData->getMaterial().texcoord.push_back( koord );
		ltp++;
		fscanf( plik, " , " ); // uwaga: przecinka moďż˝e nie byďż˝
	}
	fscanf( plik, " ] " );

	return ltp;
}
