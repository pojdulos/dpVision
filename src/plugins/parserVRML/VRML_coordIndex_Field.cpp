#include "stdafx.h"

#include "ParserWRL.h"

size_t CParserWRL::VRML_ParseField_coordIndex( FILE *plik )
{
	size_t a, b, c;
	size_t lb=0;

	CFace face;
	CVector3f vn;

	/* moze zawiera�: [ 0 1 2 -1, 3 4 5 -1, 6 7 8 -1, 9 10 11 -1 ] */
	/* albo: [ 0, 1, 2, -1, 3, 4, 5, -1, 6, 7, 8, -1, 9, 10, 11, -1 ] */

	fscanf( plik, " [ " );
	
	bool koniec = false;
//	char bufor[80];

	UI::STATUSBAR::printf( "Reading CoordIndex section..." );

	while ( ! koniec )
	{
		//fgets( bufor, 80, plik );

		bool ok;

#ifdef WIN64
		ok = 1 == fscanf( plik, " %llu ", &a );
#else
		ok = 1 == fscanf(plik, " %lu ", &a);
#endif

		fscanf( plik, " , " ); // tu mo�e nie by� przecinka

#ifdef WIN64
		ok &= 1 == fscanf(plik, " %llu ", &b);
#else
		ok &= 1 == fscanf(plik, " %lu ", &b);
#endif

		fscanf( plik, " , " ); // tu mo�e nie by� przecinka

#ifdef WIN64
		ok &= 1 == fscanf(plik, " %llu ", &c);
#else
		ok &= 1 == fscanf(plik, " %lu ", &c);
#endif

		fscanf( plik, " , " ); // tu mo�e nie by� przecinka
		fscanf( plik, " -1 " );

		face.Set( a, b, c );
		
		vn = face.getNormal( pMeshData->vertices() ); //NormalOfFace( face );

		char ch;

		UI::STATUSBAR::printfTimed( 500, "Reading: %d, F[%d,%d,%d]", lb, a, b, c );

		if ( ok )
		{
			size_t m = MAX3( a, b, c );
			if ( pMeshData->vnormals().size() <= m ) pMeshData->vnormals().resize( m+1 );

			pMeshData->vnormals()[a] += vn;
			pMeshData->vnormals()[b] += vn;
			pMeshData->vnormals()[c] += vn;

			pMeshData->faces().push_back( face );
			pMeshData->fnormals().push_back( vn );

			lb++;

			fscanf( plik, " %c ", &ch );
		}
		else
		{
			do { ch = getc(plik); } while ( ']' != ch );
		}
		
		if ( ']' == ch )
		{
			koniec = true;
		}
		else if ( ',' == ch )
		{
			;
		}
	}
	
	return lb;
}
