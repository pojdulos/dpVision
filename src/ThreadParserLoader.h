#pragma once

#ifdef DPVISION_PLUGIN_DLL
#define MYDECL Q_DECL_IMPORT
#else
#define MYDECL Q_DECL_EXPORT
#endif

#include <QtCore/QObject>

class CBaseObject;

class MYDECL ThreadParserLoader : public QObject
{
	Q_OBJECT

public:
	ThreadParserLoader(QString filename, QObject* parent = 0);
	~ThreadParserLoader();

	virtual CBaseObject* getChild();

protected:
	bool cancelled;
	CBaseObject* m_child;

public slots:
	virtual void createChild();
	virtual void doWork();
	virtual void cancelWork();

signals:
	void workDone();
	void workCancelled();
	void sendProgress(int);
};
