#include "stdafx.h"

#include "ParserWRL.h"

#include "../api/adapters/PluginUIAPIAdapter.h"

namespace {
PluginUIAPIAdapter& uiApi()
{
    static PluginUIAPIAdapter api;
    return api;
}
}

size_t CParserWRL::VRML_ParseField_point( FILE *plik )
{
	size_t lp=0;
	double ax, ay, az;

	uiApi().statusBar().setText("Reading point section...");

	fscanf( plik, " , " );

	while ( fscanf( plik, "%lf %lf %lf", &ax, &ay, &az ) == 3 )
	{
		pMeshData->vertices().push_back( CVertex( ax, ay, az ) );

		pMeshData->expandBoundingBox( CPoint3f( ax, ay, az ) );

		lp++;

		//do { ch = getc(plik); } while ( ',' != ch );
		
		fscanf( plik, " , " );
	}
	fscanf( plik, " ] " );

	return lp;
}
