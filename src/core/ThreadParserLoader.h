#pragma once

#include "dll_global.h"

#include <QtCore/QObject>

class CBaseObject;

class DPVISION_EXPORT2 ThreadParserLoader : public QObject
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
