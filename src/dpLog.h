#pragma once
#ifndef _LOG_H_BY_DP_INCLUDED_
#define _LOG_H_BY_DP_INCLUDED_

#include <QtCore/QString>
#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include "MainApplication.h"
#include "AP.h"

class __declspec(dllexport) dpLog
{
	QFile m_logFile;
	QTextStream m_txtStream;

	dpLog(void);
	~dpLog(void);
public:
	static dpLog* open(QString fname, QString prefix= AP::mainApp().appDataDir());

	void close();
	void info(QString msg);
	void info(const char* format, ...);
};

#endif /* _LOG_H_BY_DP_INCLUDED_ */