#include "ThreadParserLoader.h"

ThreadParserLoader::ThreadParserLoader(QString filename, QObject* parent) : QObject(parent)
{
	cancelled = false;
	m_child = nullptr;
}

ThreadParserLoader::~ThreadParserLoader()
{
}

CBaseObject* ThreadParserLoader::getChild()
{
	return m_child;
}

void ThreadParserLoader::cancelWork()
{
	cancelled = true;
}


void ThreadParserLoader::doWork()
{
	static int counter;

	if (cancelled)
	{
		emit(workCancelled());
	}
	else
	{
		_sleep(1000);

		counter--;

		if (counter <= 0)
		{
			emit(workDone());
		}
		else
		{
			int progress = 100 - counter;
			emit(sendProgress(progress));
		}
	}
}

void ThreadParserLoader::createChild()
{
}
