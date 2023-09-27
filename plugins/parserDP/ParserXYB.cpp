#include "StdAfx.h"

#include "ParserXYB.h"
#include "Utilities.h"

CParserXYB::CParserXYB()
{
	setDescr("Binary 3D scanner files");
	m_exts.insert( "xyb" ); 
}

CParserXYB::~CParserXYB(void)
{
}

size_t CParserXYB::Run()
{
	//format jeszcze nierozpoznany - wyskakujê
	return 0;
	
	if ( this->bIsNotSet ) return 0;

	m_model->addChild( pMeshData = new CMesh( m_model ) );
	if ( pMeshData == NULL ) return 0;

	FILE *plik;

	std::string test = plikSiatki.absoluteFilePathA().c_str();
	plik = fopen( plikSiatki.absoluteFilePathA().c_str(), "rb" );
	if ( !plik ) return 0;

	size_t lbv=0;

	float v[3];
	GLuint c[3];
	char bufor[255];

	CVertex vertex;
	CRGBA vcolor;

	fread( bufor, 101, 1, plik );

//	int tst = 0;
	while ( !feof( plik ) )
	{
		if ( ! ( lbv % 768 ) )
		{
			UI::STATUSBAR::printf( "Reading: %d", lbv );
		}

		fread( bufor, 26, 1, plik );

		typedef byte mytype;

		mytype *p0 = (mytype*) &bufor[0];
		mytype *p1 = (mytype*) &bufor[1];
		mytype *p2 = (mytype*) &bufor[2];
		mytype *p3 = (mytype*) &bufor[3];
		mytype *p4 = (mytype*) &bufor[4];
		mytype *p5 = (mytype*) &bufor[5];
		mytype *p6 = (mytype*) &bufor[6];
		mytype *p7 = (mytype*) &bufor[7];
		mytype *p8 = (mytype*) &bufor[8];
		mytype *p9 = (mytype*) &bufor[9];
		mytype *p10 = (mytype*) &bufor[10];
		mytype *p11 = (mytype*) &bufor[11];
		mytype *p12 = (mytype*) &bufor[12];
		mytype *p13 = (mytype*) &bufor[13];
		mytype *p14 = (mytype*) &bufor[14];
		mytype *p15 = (mytype*) &bufor[15];
		mytype *p16 = (mytype*) &bufor[16];
		mytype *p17 = (mytype*) &bufor[17];
		mytype *p18 = (mytype*) &bufor[18];

//		if ( tst != 6 )
//		{
//			gStatusLine.Set( "File format error." );
//			fclose( plik );
//			return lbv;
//		}

		pMeshData->expandBoundingBox( CPoint3f(v[0], v[1], v[2]) );


		vertex.Set( v[0], v[1], v[2] );

		pMeshData->vertices().push_back( vertex );

		vcolor.Set( c[0], c[1], c[2], 1 );

		pMeshData->vcolors().push_back( vcolor );

		lbv++;

	}
	fclose( plik);

	UI::STATUSBAR::printf( "Done." );

	return lbv;
}
