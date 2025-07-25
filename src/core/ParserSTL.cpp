
#include "ParserSTL.h"
#include "Utilities.h"

#include "StatusBarManager.h"

CParserSTL::CParserSTL(void)
{
	this->m_bOK = true;
}

CParserSTL::~CParserSTL(void)
{
}


size_t CParserSTL::ReadBinarySTL()
{
	// FILE *plik;
	// plik = _wfopen( this->plikSiatki.absoluteFilePathW().c_str(), L"rb" );
#ifdef _WIN32
    FILE* plik = _wfopen(this->plikSiatki.absoluteFilePathW().c_str(), L"rb");
#else
    QByteArray pathBytes = plikSiatki.absoluteFilePath().toUtf8();
    FILE* plik = fopen(pathBytes.constData(), "r");
#endif


	if (!plik) return 0;
  
	GLuint lb = 0;
	char bufor[80];

	// 80 bajt�w nag��wka, zwykle jest tam nazwa pliku i zera - pomijam
	fread( bufor, 1, 80, plik );

	// uint (4 bajty) zawieraj�cy liczb� �cian do wczytania
	fread( &lb, 4, 1, plik );

	pMeshData->vertices().clear();
	pMeshData->faces().clear();

	SVertex va, vb, vc;
	CVector3f vna, vnb, vnc;
	CFace f;
	CVector3f vn;
	SStlFace t;

	std::map< CVertex, size_t, ltCVertex >::iterator it;

	char napis[255];

	int p=0;
	StatusBarManager::setText( QString("Reading faces: %1%%").arg(p));
	long g = 1 + lb/100;

	GLuint numV = 0;
	for ( GLuint l = 0; l<lb; l++ )
	{
		if ( l%g == 0 )
		{
			StatusBarManager::setText( QString("Reading faces: %1%% ( %2 / %3 )").arg(p).arg(l).arg(lb));
			p++;
		}

		// cztery tr�jki liczb typu float (czterobajtowe)
		// najpierw wektor normalny a nast�pnie trzy wierzcho�ki
		fread( &t, 48, 1, plik );

		// te dwa bajty s� zwykle pomijane, ale mog� zawiera�
		// np. informacj� o kolorze. Na razie nie korzystam z nich
		fread( bufor, 2, 1, plik );

		va.v = t.a;
		vb.v = t.b;
		vc.v = t.c;

		it = mMapa.find( va.v );
		if ( it != mMapa.end() )
		{
			va.iId = it->second;
		}
		else
		{
			mMapa[ va.v ] = va.iId = numV++;
			pMeshData->vertices().push_back( va.v );
		}

		it = mMapa.find( vb.v );
		if ( it != mMapa.end() )
		{
			vb.iId = it->second;
		}
		else
		{
			mMapa[ vb.v ] = vb.iId = numV++;
			pMeshData->vertices().push_back( vb.v );
		}


		it = mMapa.find( vc.v );
		if ( it != mMapa.end() )
		{
			vc.iId = it->second;
		}
		else
		{
			mMapa[ vc.v ] = vc.iId = numV++;
			pMeshData->vertices().push_back( vc.v );
		}

		vn = t.n;
		f.Set( va.iId, vb.iId, vc.iId );

		try {
			pMeshData->faces().push_back( f );
			pMeshData->fnormals().push_back( vn );
		}
		catch (std::bad_alloc const&) {
			sprintf( napis, "Memory allocation fail on %lu face).", l+1 );
			this->m_sError = napis;
			this->m_bOK = false;
			pMeshData->vertices().pop_back();
			pMeshData->vertices().pop_back();
			pMeshData->vertices().pop_back();
			lb = l;
			break;
		}

		//pMeshData->calcMin( CPoint3f(	MIN3( va.v.X(), vb.v.X(), vc.v.X() ),
		//								MIN3( va.v.Y(), vb.v.Y(), vc.v.Y() ),
		//								MIN3( va.v.Z(), vb.v.Z(), vc.v.Z() ) ) );
		//
		//pMeshData->calcMax( CPoint3f(	MAX3( va.v.X(), vb.v.X(), vc.v.X() ),
		//								MAX3( va.v.Y(), vb.v.Y(), vc.v.Y() ),
		//								MAX3( va.v.Z(), vb.v.Z(), vc.v.Z() ) ) );

		pMeshData->expandBoundingBox(va.v);
		pMeshData->expandBoundingBox(vb.v);
		pMeshData->expandBoundingBox(vc.v);

	}

	fclose( plik);

	StatusBarManager::setText( "Clearing mMap..." );

	mMapa.clear();

	StatusBarManager::setText( "Creating ViewLists..." );
	return lb;
}

size_t CParserSTL::ReadTextSTL()
{
	// FILE *plik;
	// plik = _wfopen( this->plikSiatki.absoluteFilePathW().c_str(), L"r" );
#ifdef _WIN32
    FILE* plik = _wfopen(this->plikSiatki.absoluteFilePathW().c_str(), L"r");
#else
    QByteArray pathBytes = plikSiatki.absoluteFilePath().toUtf8();
    FILE* plik = fopen(pathBytes.constData(), "r");
#endif


	if (!plik) return 0;
  
	//GLuint lb = 0;

	pMeshData->vertices().clear();
	pMeshData->faces().clear();

	SVertex va, vb, vc;
	CVector3f vna, vnb, vnc;
	CFace f;
	CVector3f vn;
	SStlFace t;

	float tnx, tny, tnz;
	float tax, tay, taz;
	float tbx, tby, tbz;
	float tcx, tcy, tcz;

	std::map< CVertex, size_t, ltCVertex >::iterator it;

	char napis[255];
	char bufor[255], nazwa[80];
	
	size_t numV = 0;
	size_t l = 0;
	int result;

	fgets( bufor, 255, plik );
	result = sscanf( bufor, " solid %s ", nazwa ); //solid nazwa

	while ( !feof( plik ) )
	{
		if ( l%1000 == 0 )
		{
			StatusBarManager::setText( QString("Reading faces: %1").arg(l));
		}

		fgets( bufor, 255, plik );
		result = sscanf( bufor, " endsolid %s ", bufor, nazwa ); //solid nazwa
		if (result) break;

		sscanf( bufor, " facet normal %f %f %f\r\n", &tnx, &tny, &tnz );

		fgets( bufor, 255, plik ); //outer loop
		fscanf( plik, " vertex %f %f %f\r\n", &tax, &tay, &taz );
		fscanf( plik, " vertex %f %f %f\r\n", &tbx, &tby, &tbz );
		fscanf( plik, " vertex %f %f %f\r\n", &tcx, &tcy, &tcz );
		fgets( bufor, 255, plik ); //endloop
		fgets( bufor, 255, plik ); //endfacet

		va.v.Set( tax, tay, taz );
		vb.v.Set( tbx, tby, tbz );
		vc.v.Set( tcx, tcy, tcz );

		it = mMapa.find( va.v );
		if ( it != mMapa.end() )
		{
			va.iId = it->second;
		}
		else
		{
			mMapa[ va.v ] = va.iId = numV++;
			pMeshData->vertices().push_back( va.v );
		}

		it = mMapa.find( vb.v );
		if ( it != mMapa.end() )
		{
			vb.iId = it->second;
		}
		else
		{
			mMapa[ vb.v ] = vb.iId = numV++;
			pMeshData->vertices().push_back( vb.v );
		}


		it = mMapa.find( vc.v );
		if ( it != mMapa.end() )
		{
			vc.iId = it->second;
		}
		else
		{
			mMapa[ vc.v ] = vc.iId = numV++;
			pMeshData->vertices().push_back( vc.v );
		}

		vn.Set( tnx, tny, tnz );
		f.Set( va.iId, vb.iId, vc.iId );

		try {
			pMeshData->faces().push_back( f );
			pMeshData->fnormals().push_back( vn );
		}
		catch (std::bad_alloc const&) {
			sprintf( napis, "Memory allocation fail on %zu face).", l+1 );
			this->m_sError = napis;
			this->m_bOK = false;
			pMeshData->vertices().pop_back();
			pMeshData->vertices().pop_back();
			pMeshData->vertices().pop_back();
			break;
		}

		//pMeshData->calcMin(CPoint3f(MIN3(va.v.X(), vb.v.X(), vc.v.X()),
		//	MIN3(va.v.Y(), vb.v.Y(), vc.v.Y()),
		//	MIN3(va.v.Z(), vb.v.Z(), vc.v.Z())));

		//pMeshData->calcMax(CPoint3f(MAX3(va.v.X(), vb.v.X(), vc.v.X()),
		//	MAX3(va.v.Y(), vb.v.Y(), vc.v.Y()),
		//	MAX3(va.v.Z(), vb.v.Z(), vc.v.Z())));

		pMeshData->expandBoundingBox(va.v);
		pMeshData->expandBoundingBox(vb.v);
		pMeshData->expandBoundingBox(vc.v);

		l++;
	}

	fclose( plik);

	StatusBarManager::setText( "Clearing mMap..." );

	mMapa.clear();

	StatusBarManager::setText( "Creating ViewLists..." );
	return l;
}

size_t CParserSTL::Run()
{
	if ( this->bIsNotSet ) return 0;

	m_model->addChild(m_model, pMeshData = std::make_shared<CMesh>() );
	if ( pMeshData == NULL ) return 0;

	// najpierw trzeba sprawdzi� czy tekstowy czy binarny, w tym celu pobieram pierwsz� lini� lub 80 bajt�w
	char bufor[80];
	
	// FILE *plik = _wfopen( this->plikSiatki.absoluteFilePathW().c_str(), L"r" );
#ifdef _WIN32
    FILE* plik = _wfopen(this->plikSiatki.absoluteFilePathW().c_str(), L"r");
#else
    QByteArray pathBytes = plikSiatki.absoluteFilePath().toUtf8();
    FILE* plik = fopen(pathBytes.constData(), "r");
#endif

	
	if ( ! plik ) return 0;

	fgets( bufor, 80, plik );
	fclose( plik );

	size_t lbs = 0;

	// teraz sprawdzam, czy pierwsze 5 znak�w == 'solid'
	// w ka�dym innym wypadku przyjmuj�, ze to jest binarny stl
	if ( (bufor[0]=='s') && (bufor[1]=='o') && (bufor[2]=='l') && (bufor[3]=='i') && (bufor[4]=='d') )
	{
		lbs = ReadTextSTL();
	}
	else
	{
		lbs = ReadBinarySTL();
	}

	pMeshData->setPath(plikSiatki.absoluteFilePath());

	return lbs;
}
