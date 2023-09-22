#pragma once

#include "Parser.h"

class __declspec(dllexport) CFileConnector
{
public:
	static bool save( CModel3D *m, QString path );

	static QString getExtByFileContent(QString path);
	static CParser * getLoadParser( QString path );
	static CParser * getSaveParser( QString path );

	static QString getLoadExts();
	static QString getSaveExts();


	static void regParser( CParser *p ); 
	static void unregParser( QString ext );

private:
	static std::set<CParser*> m_parsers;

	CFileConnector(void);
	~CFileConnector(void);

	static void SavePMT( CModel3D *m, const char* nazwa );
	static void SaveXML( CModel3D *m, const char* nazwa );
};
