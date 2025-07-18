#include "stdafx.h"

#include "ParserWRL.h"
#include "Utilities.h"
#include "UI.h"

CParserWRL::CParserWRL(void)
{
	setDescr("VRML Files");
	m_exts.insert( "wrl" ); 
	m_exts.insert( "wrml" ); 
}

CParserWRL::~CParserWRL(void)
{
}


size_t CParserWRL::Run()
{
	if ( this->bIsNotSet ) return 0;

	m_model->addChild(m_model, pMeshData = std::make_shared<CMesh>() );
	if ( pMeshData == NULL ) return 0;

	return ReadVRML();
}


bool CParserWRL::VRML_FindChar( FILE *plik, char znak )
{
	char ch; 
	while ( ( ch = fgetc(plik) ) != EOF ) 
	{       
		if ( ch == znak )
			return true;
		
		if ( ch == '#')
		{
			if( ! VRML_GoToNextLine( plik ) )
				return false;
			continue;
		}
		
		if ( ch != '\n' && ch != '\r' && ch != ' ' && ch != '\t')
			return false;
	 }
	return false;
}



bool CParserWRL::VRML_IgnoreNode( FILE *plik, char ch1, char ch2 )
{
	unsigned int bO=0, bC=0;
    int ch;

	while ( ( ch = fgetc( plik ) ) != EOF )
	{    
		if ( ch == '#')
		{
			if ( ! VRML_GoToNextLine( plik ) ) return false;
		}
		else if( ch == ch1 )
		{
			bO++;
		}
		else if( ch == ch2 )
		{
			bC++;
			if ( bC > bO ) return true;
		}
    };
    return false;
}

char CParserWRL::VRML_SkipWhiteChars( FILE *plik )
{
	char ch;

	do
	{
		ch = fgetc( plik );
	}
	while ( ( ch == ' ' ) || ( ch == '\t' ) || (ch == '\r') || (ch == '\n') );

	return ch;
}


bool CParserWRL::VRML_GoToNextLine( FILE *plik )
{
	char ch;

	while ( ( ch = fgetc( plik ) ) != EOF )
	{
		if ( ch == '\n' ) return true;
	}
	
	return false;
}


size_t CParserWRL::ReadVRML()
{
	//std::string buffer = UI::stream2string( plikSiatki.absoluteFilePathW().c_str() );

#ifdef _WIN32
    FILE* plik = _wfopen(plikSiatki.absoluteFilePathW().c_str(), L"r");
#else
    QByteArray pathBytes = plikSiatki.absoluteFilePath().toUtf8();
    FILE* plik = fopen(pathBytes.constData(), "r");
#endif

	if ( ! plik ) return 0;
  
	char a1[9], a2[9], a3[9];
	fscanf( plik, "%s %s %s", a1, a2, a3 );
	// oczekujemy linii: #VRML V2.0 utf8

	if (strcmp(a1, "#VRML"))
	{
		// brak nag��wka pliku vrml - wyskakujemy
		fclose(plik);
		return 0;
	}

	if (!strcmp(a2, "V1.0"))
	{
		m_vrmlVersion = 1;

	}
	else if (!strcmp(a2, "V2.0"))
	{
		m_vrmlVersion = 2;
	}
	else
	{
		// nieobs�ugiwana wersja
		fclose(plik);
		return 0;
	}

	//int not_vrml1 = strcmp(a1, "#VRML") + strcmp(a2, "V1.0") + strcmp(a3, "ascii");
	//int not_vrml2 = strcmp(a1, "#VRML") + strcmp(a2, "V2.0") + strcmp(a3, "utf8");

	//if ( not_vrml1 && not_vrml2 )
	//{
	//	fclose( plik );
	//	return 0;
	//}

	VRML_Parse_Root( plik );

	fclose( plik );

	UI::STATUSBAR::printf(L"Ready: F:%lu, V:%lu", pMeshData->faces().size(), pMeshData->vertices().size() );

	return pMeshData->faces().size(); //faceCount;
}

