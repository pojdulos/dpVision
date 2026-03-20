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

size_t CParserWRL::VRML_ParseField_coordIndex( FILE *plik )
{
	unsigned int a = 0, b = 0, c = 0;
	size_t lb=0;

	CFace face;
	CVector3f vn;

	/* moze zawieraďż˝: [ 0 1 2 -1, 3 4 5 -1, 6 7 8 -1, 9 10 11 -1 ] */
	/* albo: [ 0, 1, 2, -1, 3, 4, 5, -1, 6, 7, 8, -1, 9, 10, 11, -1 ] */

	fscanf( plik, " [ " );
	
	bool koniec = false;
//	char bufor[80];

	uiApi().statusBar().setText("Reading CoordIndex section...");

	while ( ! koniec )
	{
		//fgets( bufor, 80, plik );

		bool ok;

#ifdef WIN64
		ok = 1 == fscanf( plik, " %u ", &a );
#else
		ok = 1 == fscanf(plik, " %u ", &a);
#endif

		fscanf( plik, " , " ); // tu moďż˝e nie byďż˝ przecinka

#ifdef WIN64
		ok &= 1 == fscanf(plik, " %u ", &b);
#else
		ok &= 1 == fscanf(plik, " %u ", &b);
#endif

		fscanf( plik, " , " ); // tu moďż˝e nie byďż˝ przecinka

#ifdef WIN64
		ok &= 1 == fscanf(plik, " %u ", &c);
#else
		ok &= 1 == fscanf(plik, " %u ", &c);
#endif

		fscanf( plik, " , " ); // tu moďż˝e nie byďż˝ przecinka
		fscanf( plik, " -1 " );

		face.Set(static_cast<INDEX_TYPE>(a), static_cast<INDEX_TYPE>(b), static_cast<INDEX_TYPE>(c));
		
		vn = face.getNormal( pMeshData->vertices() ); //NormalOfFace( face );

		char ch;

		uiApi().statusBar().setTimedText(
            500,
            QString("Reading: %1, F[%2,%3,%4]").arg(lb).arg(a).arg(b).arg(c));

		if ( ok )
		{
			size_t m = MAX3( static_cast<size_t>(a), static_cast<size_t>(b), static_cast<size_t>(c) );
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
