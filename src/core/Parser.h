#pragma once


#include "Global.h"
#include "DPFileInfo.h"

#include "Model3D.h"

class DPVISION_EXPORT2 CParser
{
protected:
	// old style -- deprecated
	bool bIsNotSet;

	// old style -- deprecated
	std::shared_ptr<CModel3D> m_model;

	// old style -- deprecated
	CFileInfo plikSiatki;

	// old style -- deprecated
	std::shared_ptr<CMesh> pMeshData;


	bool m_bOK;
	std::string m_sError;

	QString m_descr;
	QSet<QString> m_exts;
	QSet<QString> m_save_exts;

	// old style -- deprecated
	virtual size_t Run() { return 0; };

	// old style -- deprecated
	virtual bool save() { return false; };

	inline void setDescr(const QString descr) { m_descr = descr; }
	
public:
	CParser(void);
	virtual ~CParser(void);

	virtual std::shared_ptr<CModel3D> load( const QString path, bool wait = true);
	virtual bool save( std::shared_ptr<CModel3D> obj, const QString path );
	virtual bool save(QVector<std::shared_ptr<CBaseObject>> objects, const QString path) { return false; };

	virtual bool canLoadExt(const QString ext);
	virtual bool canSaveExt(const QString ext);

	virtual void getLoadExts(QString& ext);
	virtual void getLoadExts(std::wstring& ext);
	virtual void getSaveExts(QString& ext);
	virtual void getSaveExts(std::wstring& ext);

	virtual bool inPlugin() { return true; };

	//const std::wstring &getDescr() { return m_descr; };
	//void set( std::wstring npl, CModel3D *obj );

	bool IsOK() { return m_bOK; }
	std::string LastError() { return m_sError; }
};

