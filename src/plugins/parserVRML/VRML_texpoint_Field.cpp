#include "stdafx.h"

#include "ParserWRL.h"

#include "UI.h"

size_t CParserWRL::VRML_ParseField_texpoint( FILE *plik )
{
	float s, t;
	size_t ltp = 0;

	CTCoord koord;

	UI::STATUSBAR::printf( "Reading TextPoint section..." );

	while ( fscanf( plik, " %f %f ", &s, &t ) == 2)
	{
		koord[0] = s;
		koord[1] = t;
		pMeshData->getMaterial().texcoord.push_back( koord );
		ltp++;
		fscanf( plik, " , " ); // uwaga: przecinka mo�e nie by�
	}
	fscanf( plik, " ] " );

	return ltp;
}
