
#include "Parser.h"

CParser::CParser()
{
	setDescr("Sample description");
	m_exts.clear(); 

	bIsNotSet = true;
	m_bOK = true;
	m_sError.clear();
}

CParser::~CParser(void)
{
}

std::shared_ptr<CModel3D> CParser::load(const QString path, bool wait)
{
	std::shared_ptr<CModel3D> obj;

	// allocate
	try {
		obj = std::make_shared<CModel3D>();
	}
	catch (std::bad_alloc & e) {
		return nullptr;
	}

	//------------------------------------------------------
	// set
	// this is needed for Run() function
	this->m_model = obj;
	this->plikSiatki.setPath( path );
	this->pMeshData = NULL;
	this->bIsNotSet = false;
	//------------------------------------------------------

	obj->setLabel(QFileInfo(path).fileName());

	//------------------------------------------------------
	// run
	// calling Run() for old parsers compatibility
	int lb = Run();
	//------------------------------------------------------


	if (lb > 0)
	{
		obj->setPath( path );
		//obj->setLabel(QFileInfo(path).fileName());

		if (nullptr != obj->getChild())
		{
			obj->getChild()->afterLoad(obj.get());
			obj->importChildrenGeometry();

			return obj;
		}
		else
		{
			UI::MESSAGEBOX::error(L"Nie udało sie wczytać pliku");
		}
	}

	//delete obj;
	return nullptr;
}

bool CParser::save(std::shared_ptr<CModel3D> obj, const QString path)
{
	//------------------------------------------------------
	// set
	// this is needed for old save() function
	this->m_model = obj;
	this->plikSiatki.setPath( path );
	this->pMeshData = nullptr;
	this->bIsNotSet = false;
	//------------------------------------------------------

	return save();
}

bool CParser::canLoadExt(const QString ext)
{
	return ( m_exts.find(ext) != m_exts.end());
}

bool CParser::canSaveExt(const QString ext)
{
	return (m_save_exts.find(ext) != m_save_exts.end());
}

void CParser::getLoadExts(QString& ext)
{
	if (!m_exts.empty())
	{
		if (!ext.isEmpty())
		{
			ext.append(";;");
		}

		ext.append(m_descr + " (");

		QSet<QString>::iterator its;
		
		for (its = m_exts.begin(); its != m_exts.end(); its++)
		{
			ext.append( "*." + (*its) + ";" );
		}

		ext = ext.left(ext.length() - 1);
		ext.append( ")" );
	}
}

void CParser::getLoadExts(std::wstring &ext)
{
	QString temp = QString::fromStdWString(ext);

	getLoadExts(temp);

	ext = temp.toStdWString();
}

void CParser::getSaveExts(QString& ext)
{
	if (! m_save_exts.empty())
	{
		if (!ext.isEmpty())
		{
			ext.append(";;");
		}

		ext.append(m_descr + " (");

		QSet<QString>::iterator its;
		for (its = m_exts.begin(); its != m_exts.end(); its++)
		{
			ext.append( "*." + (*its) + ";" );
		}

		ext = ext.left(ext.length() - 1);
		ext.append( ")" );
	}
}

void CParser::getSaveExts(std::wstring& ext)
{
	QString temp = QString::fromStdWString(ext);

	getSaveExts(temp);

	ext = temp.toStdWString();
}

//void CParser::set( std::wstring npl, CModel3D *obj )
//{
//	this->m_model = obj;
//
//	this->plikSiatki.SetFullPath( npl );
//	this->pMeshData = NULL;
//
//	this->bIsNotSet = false;
//}
