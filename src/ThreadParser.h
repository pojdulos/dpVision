#pragma once

#ifdef DPVISION_PLUGIN_DLL
#define MYDECL Q_DECL_IMPORT
#else
#define MYDECL Q_DECL_EXPORT
#endif

#include "Parser.h"

#include <QtCore/QObject>
#include <QtCore/QFileInfo>

class ThreadParserLoader;
class QThread;

class MYDECL ThreadParser : public QObject, public CParser
{
	Q_OBJECT

public:
	explicit ThreadParser(void);

	virtual CModel3D* load(QString path, bool wait = false) override;
	virtual bool save(CModel3D* obj, QString path) override;

protected:
	ThreadParserLoader* loader;
	QThread* thread;

	QFileInfo fileinfo;

	bool synchronous;
	bool done;
	bool cancelled;

	void runLoader();


public slots:
	virtual void onLoadDone();
	virtual void onLoadCancelled();

// hide some deprecated members
private:
	using CParser::Run;
	using CParser::plikSiatki;
	using CParser::bIsNotSet;
	using CParser::pMeshData;
};
