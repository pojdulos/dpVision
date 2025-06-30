#include "stdafx.h"

#include "ParserWRL.h"

size_t CParserWRL::VRML_ParseField_point( FILE *plik )
{
	size_t lp=0;
	double ax, ay, az;
	char ch;

	UI::STATUSBAR::printf( "Reading point section..." );

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
