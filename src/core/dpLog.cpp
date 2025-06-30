#include "dpLog.h"

dpLog::dpLog(void){}
dpLog::~dpLog(void){}

dpLog* dpLog::open(QString fname, QString prefix)
{
	//QString prefix = AP::mainApp().appDataDir();

	dpLog* newLog = new dpLog();
	newLog->m_logFile.setFileName(prefix + QDir::separator() + fname);
	if (newLog->m_logFile.open(QIODevice::Append | QIODevice::WriteOnly))
	{
		newLog->m_txtStream.setDevice(&newLog->m_logFile);
		return newLog;
	}

	delete newLog;
	return nullptr;
}

void dpLog::close()
{
	if (m_logFile.isOpen())
		m_logFile.close();
}

void dpLog::info(QString msg)
{
	m_txtStream << msg << Qt::endl;
}

void dpLog::info(const char* format, ...)
{
	va_list paramList;
	va_start(paramList, format);

	//	vprintf_s(format, paramList);
	char formatBuf[1024];
	// vsprintf_s(formatBuf, _countof(formatBuf), format, paramList);
	vsnprintf(formatBuf, sizeof(formatBuf), format, paramList);


	m_txtStream << formatBuf << Qt::endl;
}

