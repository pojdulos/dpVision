#include "FileConnector.h"

CFileConnector::CFileConnector(void)
{
}


CFileConnector::~CFileConnector(void)
{
}

std::set<CParser*> CFileConnector::m_parsers;

bool CFileConnector::save( CModel3D *m, QString path )
{
	QFileInfo finfo( path );

	if (0 == finfo.suffix().compare("xml", Qt::CaseInsensitive) )
	{
		SaveXML( m, path.toStdString().c_str() );
	}
	else
	{
		SavePMT( m, path.toStdString().c_str() );
	}

	m->setPath( path );
	return true;
}

void CFileConnector::SavePMT( CModel3D *m, const char* nazwa )
{
	UI::STATUSBAR::printf( "PMFactory::SaveNewFormSMF() is writing to file (ver.5) now, please wait..." );

	CMesh *src = (CMesh*)m->getChild().get();

	FILE *plik = fopen( nazwa, "w" );
	
	size_t matIdx = 0;

	CVector3d e = m->getTransform().rotation().eulerAnglesDeg();

	fprintf( plik, "# PM wersja pliku 05.14-21.00\n$m 5\n$o %d\n", src->orgsize );
	fprintf( plik, "$v %zd\n$f %zd\n$s %zd\n", src->vertices().size(), src->faces().size(), src->vsplits.size() );
	fprintf( plik, "$i %zd\n$c %zd\n$n %s\n", src->getMaterial(matIdx).texindex.size(), src->getMaterial(matIdx).texcoord.size(), src->getMaterial(matIdx).TexInfo );
	fprintf( plik, "$r %f,%f,%f\n", e.x, e.y, e.z );
	fprintf( plik, "$t %f,%f,%f\n", m->getTransform().translation().X(), m->getTransform().translation().Y(), m->getTransform().translation().Z() );
	fprintf( plik, "$p 1.0\n" );
	fprintf( plik, "#\n" );

	for ( CMesh::Vertices::iterator itv=src->vertices().begin(); itv!=src->vertices().end(); itv++ )
	{
		fprintf( plik, "v %f %f %f\n", itv->X(), itv->Y(), itv->Z() );
	}

	for ( CMesh::Faces::iterator itf=src->faces().begin(); itf!=src->faces().end(); itf++ )
	{
		fprintf( plik, "f %d %d %d\n", itf->A(), itf->B(), itf->C() );
	}

	for ( CMaterial::TextureIndexes::iterator iti=src->getMaterial(matIdx).texindex.begin(); iti!=src->getMaterial(matIdx).texindex.end(); iti++ )
	{
		fprintf( plik, "i %zd %zd %zd\n", iti->a, iti->b, iti->c );
	}

	for (CMaterial::TextureCoordinates::iterator itc=src->getMaterial(matIdx).texcoord.begin(); itc!=src->getMaterial(matIdx).texcoord.end(); itc++ )
	{
		fprintf(plik, "c %f %f\n", itc[0], itc[1]);
	}

	for ( CMesh::Vsplits::iterator its=src->vsplits.begin(); its!=src->vsplits.end(); its++ )
	{
		fprintf( plik, "s %zd %f %f %f", its->i1, its->v1.X(), its->v1.Y(), its->v1.Z() );
		fprintf( plik, " %zd %f %f %f %zd", its->i2, its->v2.X(), its->v2.Y(), its->v2.Z(), its->delFcs.size() );

		for ( _vsplit::Faces::iterator itfd=its->delFcs.begin(); itfd!=its->delFcs.end(); itfd++ )
			fprintf( plik, " %d %d %d", itfd->A(), itfd->B(), itfd->C() );

		fprintf( plik, " %zd", its->chgFcs.size() );

		for ( _vsplit::Faces::iterator itfc=its->chgFcs.begin(); itfc!=its->chgFcs.end(); itfc++ )
			fprintf( plik, " %d %d %d", itfc->A(), itfc->B(), itfc->C() );

		fprintf( plik, " %zd", its->delTI.size() );
		for ( _vsplit::TextureIndexes::iterator itti=its->delTI.begin(); itti!=its->delTI.end(); itti++ )
			fprintf( plik, " %zd %zd %zd", itti->a, itti->b, itti->c );

		fprintf( plik, " %zd", its->delTC.size() );
		for ( _vsplit::TextureCoords::iterator ittc=its->delTC.begin(); ittc!=its->delTC.end(); ittc++ )
			fprintf( plik, " %zd %f %f", ittc->first, ittc->second[0], ittc->second[1]);

		fprintf( plik, " %zd", its->chgTC.size() );
		for ( _vsplit::TextureCoords::iterator ittc=its->chgTC.begin(); ittc!=its->chgTC.end(); ittc++ )
			fprintf( plik, " %zd %f %f", ittc->first, ittc->second[0], ittc->second[1]);

		fprintf( plik, "\n" );
	}

	fclose( plik );
}

void CFileConnector::SaveXML( CModel3D *m, const char* nazwa )
{
	UI::STATUSBAR::printf( "PMFactory::SaveXML() is writing to file now, please wait..." );
	
	CMesh *src = (CMesh*)m->getChild().get();

	size_t matIdx = 0;

	FILE *plik = fopen( nazwa, "w" );
	
	CVector3d e = m->getTransform().rotation().eulerAnglesDeg();

	fprintf( plik, "<mesh orgsize=\"%d\">\n", src->orgsize ); 
 	fprintf( plik, " <rotation x=\"%f\" y=\"%f\" z=\"%f\" />\n", e.x, e.y, e.z );
 	fprintf( plik, " <translation x=\"%f\" y=\"%f\" z=\"%f\" />\n", m->getTransform().translation().X(), m->getTransform().translation().Y(), m->getTransform().translation().Z() );
 	fprintf( plik, " <scale>%f</scale>\n", m->getTransform().scale().x );

	fprintf( plik, " <basemesh>\n" );

	fprintf( plik, "  <vertices count=\"%zd\">\n", src->vertices().size() );
	size_t j=0;
	for ( CMesh::Vertices::iterator itv=src->vertices().begin(); itv!=src->vertices().end(); itv++ )
	{
		fprintf( plik, "   <vertex id=\"%zd\" x=\"%f\" y=\"%f\" z=\"%f\" />\n", j, itv->X(), itv->Y(), itv->Z() );
		j++;
	}
	fprintf( plik, "  </vertices>\n" );

	fprintf( plik, "  <faces count=\"%zd\">\n", src->faces().size() );
	for ( CMesh::Faces::iterator itf=src->faces().begin(); itf!=src->faces().end(); itf++ )
	{
		fprintf( plik, "   <face a=\"%d\" b=\"%d\" c=\"%d\" />\n", itf->A(), itf->B(), itf->C() );
	}
	fprintf( plik, "  </faces>\n" );

	fprintf( plik, " </basemesh>\n" );

	fprintf( plik, " <texture>\n" );
	fprintf( plik, "  <file>%s</file>\n", src->getMaterial(matIdx).TexInfo );

	fprintf( plik, "  <texindexes count=\"%zd\">\n", src->getMaterial(matIdx).texindex.size() );
	for ( CMaterial::TextureIndexes::iterator iti=src->getMaterial(matIdx).texindex.begin(); iti!=src->getMaterial(matIdx).texindex.end(); iti++ )
	{
		fprintf( plik, "   <ti a=\"%zd\" b=\"%zd\" c=\"%zd\" />\n", iti->a, iti->b, iti->c );
	}
	fprintf( plik, "  </texindexes>\n" );

	fprintf( plik, "  <texcoords count=\"%zd\">\n", src->getMaterial(matIdx).texcoord.size() );
	j=0;
	for ( CMaterial::TextureCoordinates::iterator itc=src->getMaterial(matIdx).texcoord.begin(); itc!=src->getMaterial(matIdx).texcoord.end(); itc++ )
	{
		fprintf( plik, "    <tc id=\"%zd\" s=\"%f\" t=\"%f\" />\n", j, itc[0], itc[1]);
		j++;
	}
	fprintf( plik, "  </texcoords>\n" );

	fprintf( plik, " </texture>\n" );

	fprintf( plik, " <vsplits count=\"%zd\">\n", src->vsplits.size() );

	j=0;
	for ( CMesh::Vsplits::iterator its=src->vsplits.begin(); its!=src->vsplits.end(); its++ )
	{
		fprintf( plik, "  <vsplit id=\"%zd\">\n", j++ );
		fprintf( plik, "   <v1 id=\"%zd\" x=\"%f\" y=\"%f\" z=\"%f\" />\n", its->i1, its->v1.X(), its->v1.Y(), its->v1.Z() );
		fprintf( plik, "   <v2 id=\"%zd\" x=\"%f\" y=\"%f\" z=\"%f\" />\n", its->i2, its->v2.X(), its->v2.Y(), its->v2.Z() );

		//---------------------------------

		if ( 0 != its->delFcs.size() )
		{
			fprintf( plik, "   <delFcs count=\"%zd\">\n", its->delFcs.size() );

			for ( _vsplit::Faces::iterator itfd=its->delFcs.begin(); itfd!=its->delFcs.end(); itfd++ )
				fprintf( plik, "    <face a=\"%d\" b=\"%d\" c=\"%d\" />\n", itfd->A(), itfd->B(), itfd->C() );

			fprintf( plik, "   </delFcs>\n" );
		}
		else
		{
//			fprintf( plik, "   <delFcs count=\"0\" />\n" );
		}

		//---------------------------------

		if ( 0 != its->chgFcs.size() )
		{
			fprintf( plik, "   <chgFcs count=\"%zd\">\n", its->chgFcs.size() );

			for ( _vsplit::Faces::iterator itfc=its->chgFcs.begin(); itfc!=its->chgFcs.end(); itfc++ )
				fprintf( plik, "    <face a=\"%d\" b=\"%d\" c=\"%d\" />\n", itfc->A(), itfc->B(), itfc->C() );

			fprintf( plik, "   </chgFcs>\n" );
		}
		else
		{
//			fprintf( plik, "   <chgFcs count=\"0\" />\n" );
		}

		//---------------------------------

		if ( 0 != its->delTI.size() )
		{
			fprintf( plik, "   <delTI count=\"%zd\">\n", its->delTI.size() );

			for ( _vsplit::TextureIndexes::iterator itti=its->delTI.begin(); itti!=its->delTI.end(); itti++ )
				fprintf( plik, "    <ti a=\"%zd\" b=\"%zd\" c=\"%zd\" />\n", itti->a, itti->b, itti->c );

			fprintf( plik, "   </delTI>\n" );
		}
		else
		{
//			fprintf( plik, "   <delTI count=\"0\" />\n" );
		}

		//---------------------------------

		if ( 0 != its->delTC.size() )
		{
			fprintf( plik, "   <delTC count=\"%zd\">\n", its->delTC.size() );

			for ( _vsplit::TextureCoords::iterator ittc=its->delTC.begin(); ittc!=its->delTC.end(); ittc++ )
				fprintf( plik, "    <tc id=\"%zd\" s=\"%f\" t=\"%f\" />\n", ittc->first, ittc->second[0], ittc->second[1]);

			fprintf( plik, "   </delTC>\n" );
		}
		else
		{
//			fprintf( plik, "   <delTC count=\"0\" />\n" );
		}

		//---------------------------------

		if ( 0 != its->chgTC.size() )
		{
			fprintf( plik, "   <chgTC count=\"%zd\">\n", its->chgTC.size() );

			for ( _vsplit::TextureCoords::iterator ittc=its->chgTC.begin(); ittc!=its->chgTC.end(); ittc++ )
				fprintf( plik, "    <tc id=\"%zd\" s=\"%f\" t=\"%f\" />\n", ittc->first, ittc->second[0], ittc->second[1]);

			fprintf( plik, "   </chgTC>\n" );
		}
		else
		{
//			fprintf( plik, "   <chgTC count=\"0\" />\n" );
		}

		//---------------------------------

		fprintf( plik, "  </vsplit>\n" );
	}

	fprintf( plik, " </vsplits>\n" );
	fprintf( plik, "</mesh>\n" );
	fclose( plik );
}

void CFileConnector::regParser( CParser *p )
{
	m_parsers.insert( p );
}

void CFileConnector::unregParser( QString ext )
{
	std::set<CParser*>::iterator it;
	
	for ( it = CFileConnector::m_parsers.begin(); it != CFileConnector::m_parsers.end(); it++ )
	{
		if ( (*it)->canLoadExt(ext) )
		{
			delete (*it);
			m_parsers.erase( it );
			return;
		}
	}
}

QString CFileConnector::getLoadExts()
{
	std::wstring ext = L"All (*.*);;Digital patient workspace (*.dpw;*.atmdl);;dpVision multiarchive (*.dpvision);;OBJ (*.obj);;STL (*.STL);;SMF (*.pmt;*.smf)";

	for (auto parser : m_parsers)
	{
		parser->getLoadExts(ext);
	}

	return QString::fromStdWString(ext);
}

QString CFileConnector::getSaveExts()
{
	std::wstring ext = L"Digital patient workspace (*.dpw;*.atmdl);;dpVision multiarchive (*.dpvision);;OBJ File (*.obj);;Progressive Mesh Text File (*.pmt);;Progressive Mesh XML File (*.xml)";

	for (auto parser : m_parsers)
	{
		parser->getSaveExts(ext);
	}

	return QString::fromStdWString(ext);
}

QString CFileConnector::getExtByFileContent(QString path)
{
	QFile plik(path);
	QString ext("");

	if (plik.open(QIODevice::ReadOnly))
	{
		plik.seek(0x80);

		QString dcm(plik.read(4));

		if (dcm.startsWith("DICM"))
			ext = "dcm";

		plik.close();
	}

	return ext;
}


#include "ParserOBJ.h"
#include "ParserSMF.h"
#include "ParserSTL.h"
#include "ParserDPV.h"
#include "ParserATMDL.h"


CParser * CFileConnector::getLoadParser( QString path )
{
	CFileInfo finfo( path );
	CParser *parser = nullptr;

	if (finfo.hasExt("dpvision"))
	{
		parser = new CParserDPVISION();
	}
	else if (finfo.hasExt("dpw") || finfo.hasExt("atmdl"))
	{
		parser = new CParserATMDL();
	}
	else if ( finfo.hasExt( "obj" ) )
	{
		parser = new CParserOBJ();
	}
	else if ( finfo.hasExt( "pmt" ) || finfo.hasExt( "smf" ) )
	{
		parser = new CParserSMF();
	}
	else if ( finfo.hasExt( "stl" ) )
	{
		parser = new CParserSTL();
	}
	else
	{
		QString ext = finfo.suffix();
		

		if (ext.isEmpty()) {
			ext = getExtByFileContent(path);
		}

		
		for (auto p : m_parsers)
		{
			if (p->canLoadExt(ext))
			{
				return p;
			}
		}

		UI::MESSAGEBOX::error( L"Unknown file extension.\n" + path.toStdWString() );
	}

	return parser;
}

CParser * CFileConnector::getSaveParser(QString path)
{
	CFileInfo finfo( path );
	CParser *parser = nullptr;

	if (finfo.hasExt("dpvision"))
	{
		parser = new CParserDPVISION();
	}
	else if (finfo.hasExt("dpw") || finfo.hasExt("atmdl"))
	{
		parser = new CParserATMDL();
	}
	else if ( finfo.hasExt("obj") )
	{
		parser = new CParserOBJ();
	}
	//else if (finfo.HasExt(L"pmt") || finfo.HasExt(L"smf"))
	//{
	//	parser = new CParserSMF();
	//}
	//else if (finfo.HasExt(L"stl"))
	//{
	//	parser = new CParserSTL();
	//}
	else
	{
		for ( auto p : m_parsers )
		{
			if ( p->canSaveExt(finfo.suffix()) )
			{
				return p;
			}
		}
	}

	return parser;
}
