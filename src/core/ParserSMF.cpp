#include "ParserSMF.h"

#include "StatusBarManager.h"

CParserSMF::CParserSMF()
{
}

CParserSMF::~CParserSMF(void)
{
}

size_t CParserSMF::Run()
{
	if ( this->bIsNotSet ) return 0;

	m_model->addChild( pMeshData = std::make_shared<CMesh>() );
	if ( pMeshData == NULL ) return 0;

	return ReadSMF_V3();
}

void CParserSMF::SkipLine( FILE *plik )
{
	char bufor[8192];
	fgets( bufor, 8192, plik );
}

void CParserSMF::ReadPartFromV4( FILE *plik, SVsplit & vsplit )
{
	int ile;
	unsigned int a, b, c;
	float s, t;

	if ( 1 == fscanf( plik, " %d ", &ile ) )
	{
		for (int i=0; i<ile; i++)
		{
			fscanf( plik, " %d %d %d ", &a, &b, &c );
			try {
				vsplit.delTI.push_back( CTIndex( a, b, c ) );
			}
			catch ( std::bad_alloc &e )
			{
				UI::MESSAGEBOX::error( e.what() );
			}
		}
	}

	if ( 1 == fscanf( plik, " %d ", &ile ) )
	{
		for (int i=0; i<ile; i++)
		{
			fscanf( plik, " %d %f %f ", &a, &s, &t );
			try {
				vsplit.delTC.push_back( _vsplit::TCoord( a, CTCoord( s, t ) ) );
			}
			catch ( std::bad_alloc &e )
			{
				UI::MESSAGEBOX::error( e.what() );
			}
		}
	}
}

void CParserSMF::ReadPartFromV5( FILE *plik, SVsplit & vsplit )
{
	int ile;
	unsigned int a;
	float s, t;

	if ( 1 == fscanf( plik, " %d ", &ile ) )
	{
		for (int i=0; i<ile; i++)
		{
			fscanf( plik, " %d %f %f ", &a, &s, &t );
			try {
				vsplit.chgTC.push_back( _vsplit::TCoord( a, CTCoord( s, t ) ) );
			}
			catch ( std::bad_alloc &e )
			{
				UI::MESSAGEBOX::error( e.what() );
			}
		}
	}
}

void CParserSMF::ReadVSplit( FILE *plik, SVsplit & vsplit )
{
	unsigned int i1, i2;
	float v1x, v1y, v1z, v2x, v2y, v2z;

	// vertex v1 ( zmieniony )
	fscanf( plik, " %d %f %f %f ", &i1, &v1x, &v1y, &v1z );
	// vertex v2 ( usuniety )
	fscanf( plik, " %d %f %f %f ", &i2, &v2x, &v2y, &v2z );

	vsplit.i1 = i1;
	vsplit.v1.Set( v1x, v1y, v1z );

	vsplit.i2 = i2;
	vsplit.v2.Set( v2x, v2y, v2z );

	int ile;
	unsigned int a, b, c;

	// sciany usuniete
	vsplit.delFcs.clear();
	fscanf( plik, " %d ", &ile );
	for (int i=0; i<ile; i++)
	{
		fscanf( plik, " %d %d %d ", &a, &b, &c );
		vsplit.delFcs.push_back( CFace( a, b, c ) );
	}

	// sciany zmienione
	vsplit.chgFcs.clear();
	fscanf( plik, " %d ", &ile );
	for (int i=0; i<ile; i++)
	{
		fscanf( plik, " %d %d %d ", &a, &b, &c );
		vsplit.chgFcs.push_back( CFace( a, b, c ) );
	}

	vsplit.delTI.clear();
	vsplit.delTC.clear();
	if ( pMeshData->version > 3 )
		ReadPartFromV4( plik, vsplit );


	vsplit.chgTC.clear();
	if ( pMeshData->version > 4 ) // dochodzi wektor chgTC
		ReadPartFromV5( plik, vsplit );

}

//************************************************
// czyta SMF v3
// bardzo szybka - lira1000f_v3.smf w 6.8s
//************************************************
size_t CParserSMF::ReadSMF_V3()
{
	if ( this->bIsNotSet ) return 0;

#ifdef _WIN32
    FILE* plik = _wfopen(plikSiatki.absoluteFilePathW().c_str(), L"r");
#else
    QByteArray pathBytes = plikSiatki.absoluteFilePath().toUtf8();
    FILE* plik = fopen(pathBytes.constData(), "r");
#endif


	if ( plik == NULL ) return 0;

	size_t lbv=0, lbf=0, lbn=0, lbi=0, lbc=0, lbvsp=0;

	float ax, ay, az;
	float s, t;
	
	CVertex vertex;
	CFace face;
	CVector3f WNorm;
	SVsplit vsplit;

	size_t vnb=0, fnb=0, snb=0, inb=0, cnb=0;

	StatusBarManager::setText( QString("I'm reading Progressive Mesh Text file: %1. Please wait...").arg(plikSiatki.fileName()));

	char c1, c2;

	while ( ! feof( plik ) )
	{
		StatusBarManager::printfTimed( 500, QString("Reading %1: v=%2, f=%3, s=%4").arg(plikSiatki.fileName()).arg(pMeshData->vertices().size()).arg(pMeshData->faces().size()).arg(pMeshData->vsplits.size()));

		c1 = fgetc( plik );
		switch( c1 )
		{
			case '$':
				c2 = fgetc( plik );

				switch ( c2 )
				{
					case 'm': //wersja siatki progresywnej
						fscanf( plik, " %d ", &(pMeshData->version) );
						if ( pMeshData->version < 3 )
						{
							fclose( plik );
							return 0; //ReadSMF();
						}
						break;
					case 'o': //liczba scian w oryginalnej siatce
						fscanf( plik, " %d ", &(pMeshData->orgsize) );
						break;
					case 'r': //rotacja
						fscanf( plik, " %f , %f , %f ", &ax, &ay, &az );
						m_model->transform().rotation().fromEulerAnglesDeg( ax, ay, az );
						break;
					case 't': //translacja
						fscanf( plik, " %f , %f , %f ", &ax, &ay, &az );
						m_model->getTransform().translation() = CVector3d(ax, ay, az );
						break;
					case 'p': //powiekszenie (skala)
						fscanf( plik, " %f ", &ax );
						// pMeshData->scale=ax; //nie mam takiego pola
						break;
					case 'v': //vertex
						fscanf( plik, " %zd ", &vnb );
						pMeshData->vertices().reserve( vnb );
						break;
					case 'f': //faces 
						fscanf( plik, " %zd ", &fnb );
						pMeshData->faces().clear();
						pMeshData->faces().reserve( fnb );
						break;
					case 'i': //texture indexes 
						fscanf( plik, " %zd ", &inb );
						pMeshData->getMaterial(0).texindex.reserve( inb );
						break;
					case 'c': //texture UV coords 
						fscanf( plik, " %zd ", &cnb );
						pMeshData->getMaterial(0).texcoord.reserve( cnb );
						break;
					case 'n': //texture filename 
						char tmp1[8192];
						fscanf( plik, " %s ", tmp1 );
						pMeshData->getMaterial(0).TexInfo = QString( tmp1 );
						break;
					case 's': //vsplits
						fscanf( plik, " %zd ", &snb );
						pMeshData->vsplits.reserve( snb );
						break;
					default:
						break;
				}
				break;

			case 'v':
				if ( fscanf( plik, " %f %f %f \n", &ax, &ay, &az ) == 3 )
				{				
					pMeshData->expandBoundingBox( CPoint3f( ax, ay, az ) );

					vertex.Set( ax, ay, az );
					pMeshData->vertices().push_back( vertex );
				
					lbv++;
				}
				break;

			case 'f': // face
				unsigned int f1v, f2v, f3v;
				if ( fscanf( plik, " %d %d %d \n", &f1v, &f2v, &f3v ) == 3 )
				{
					face.Set( f1v, f2v, f3v );
				
					pMeshData->faces().push_back( face );
					pMeshData->fnormals().push_back( face.getNormal( pMeshData->vertices() ) ); //NormalOfFace( face ) );

					lbf++;
				}
				break;

			case 'i': // texture indexes
				unsigned int t1, t2, t3;
				if ( fscanf( plik, " %d %d %d \n", &t1, &t2, &t3 ) == 3 )
				{
					pMeshData->getMaterial(0).texindex.push_back( CTIndex(t1,t2,t3) );
					lbi++;
				}
				break;

			case 'c': // texture UV coords
				if ( fscanf( plik, " %f %f \n", &s, &t ) == 2 )
				{
					pMeshData->getMaterial(0).texcoord.push_back( CTCoord(s,t) );
					lbc++;
				}
				break;

			case 's': //vsplits 
				
				ReadVSplit( plik, vsplit );

				try {
					pMeshData->vsplits.push_back( vsplit );
				}
				catch ( std::bad_alloc &e )
				{
					UI::MESSAGEBOX::error( e.what() );
				}

				lbvsp++;
				break;

			case '#': 
			default: 
				SkipLine( plik );
				break;
		}
		//UI::STATUSBAR::printf( "%s: v=%d, f=%d, s=%d", plikSiatki.fileNameW().c_str(), pMeshData->vertices().size(), pMeshData->faces().size(), pMeshData->vsplits.size() );
	}
	fclose( plik );

	pMeshData->version = 2;

	StatusBarManager::setText( "Done." );

	return lbv;
}
//************************************************


#include <fstream>

//************************************************
// tez czyta SMF v3
// proba uzycia ifstream zamiast FILE*
// okazalo sie ze operator >> dziala ponad 3x
// wolniej niz odczyt z uzyciem fscanf()
// plik lira1000f_v3.smf czyta sie ok. 23s
//************************************************

size_t CParserSMF::ReadSMF_V3b()
{
	if ( this->bIsNotSet ) return 0;

	std::ifstream in( plikSiatki.absoluteFilePathA() );
	if ( ! in.is_open() ) return 0;
	in.setf( std::ios::skipws );

//	FILE *plik = fopen( plikSiatki.absoluteFilePathW().c_str(), "r" );
//	if ( plik == NULL ) return 0;

	size_t lbv=0, lbf=0, lbn=0, lbti=0, lbtc=0, lbvsp=0;

	float ax, ay, az;
	
	CVertex vertex;
	CFace face;
	CVector3f WNorm;
	SVsplit vsplit;

	size_t vnb=0, fnb=0, snb=0;

	StatusBarManager::setText( "I'm reading Progressive Mesh Text file. Please wait..." );

	char c1, c2;

	while ( in >> c1 )
	{
		switch( c1 )
		{
			case '$':
				in >> c2;

				switch ( c2 )
				{
					case 'm': //wersja siatki progresywnej
						in >> pMeshData->version;
						if ( pMeshData->version != 3 )
						{
							in.close();
							return 0; //ReadSMF();
						}
						break;
					case 'o': //liczba scian w oryginalnej siatce
						in >> pMeshData->orgsize;
						break;
					case 'r': //rotacja
						in >> ax >> ay >> az;
						m_model->getTransform().rotation().fromEulerAnglesDeg( ax, ay, az );
						break;
					case 't': //translacja
						in >> ax >> ay >> az;
						m_model->getTransform().translation() = CVector3d(ax, ay, az );
						break;
					case 'p': //powiekszenie (skala)
						in >> ax;
						// pMeshData->scale=rx; //nie mam takiego pola
						break;
					case 'v': //vertex
						in >> vnb;
						pMeshData->vertices().reserve( vnb );
						break;
					case 'f': //faces 
						in >> fnb;
						pMeshData->faces().reserve( fnb );
						break;
					case 's': //vsplits
						in >> snb;
						pMeshData->vsplits.reserve( snb );
						break;
					default:
						break;
				}
				break;

			case 'v':
				in >> ax >> ay >> az;
				
				pMeshData->expandBoundingBox( CPoint3f( ax, ay, az ) );

				vertex.Set( ax, ay, az );
				pMeshData->vertices().push_back( vertex );
				
				lbv++;
				break;

			case 'f': // face
				unsigned int f1v, f2v, f3v;
				in >> f1v >> f2v >> f3v;

				face.Set( f1v, f2v, f3v );
				
				pMeshData->faces().push_back( face );
				pMeshData->fnormals().push_back( face.getNormal( pMeshData->vertices() ) ); //NormalOfFace( face ) );

				lbf++;
				break;

			case 's': //vsplits 
				unsigned int i1, i2;
				float v1x, v1y, v1z, v2x, v2y, v2z;

				// vertex v1 ( zmieniony )
				in >> i1 >> v1x >> v1y >> v1z;
				// vertex v2 ( usuniety )
				in >> i2 >> v2x >> v2y >> v2z;

				vsplit.i1 = i1;
				vsplit.v1.Set( v1x, v1y, v1z );

				vsplit.i2 = i2;
				vsplit.v2.Set( v2x, v2y, v2z );

				int ile;
				unsigned int a, b, c;

				// sciany usuniete
				vsplit.delFcs.clear();
				in >> ile;
				for (int i=0; i<ile; i++)
				{
					in >> a >> b >> c;
					vsplit.delFcs.push_back( CFace( a, b, c ) );
				}

				// sciany zmienione
				vsplit.chgFcs.clear();
				in >> ile;
				for (int i=0; i<ile; i++)
				{
					in >> a >> b >> c;
					vsplit.chgFcs.push_back( CFace( a, b, c ) );
				}

				pMeshData->vsplits.push_back( vsplit );
				
				lbvsp++;
				break;

			case '#': 
			default: 
				in.ignore( 1024, '\n' );
				break;
		}
	}
	in.close();

	pMeshData->version = 2;
	pMeshData->setPath(plikSiatki.absoluteFilePath());

	StatusBarManager::setText( "Done." );

	return lbf;
}



//************************************************
