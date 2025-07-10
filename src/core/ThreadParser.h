#pragma once

#include "dll_global.h"

#include "Parser.h"

#include <QtCore/QObject>
#include <QtCore/QFileInfo>

class ThreadParserLoader;
class QThread;

class DPVISION_EXPORT2 ThreadParser : public QObject, public CParser
{
	Q_OBJECT

public:
	explicit ThreadParser(void);

	virtual std::shared_ptr<CModel3D> load(QString path, bool wait = false) override;
	virtual bool save(std::shared_ptr<CModel3D> obj, const QString path) override;

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
