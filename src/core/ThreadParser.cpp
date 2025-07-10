#include "ThreadParser.h"

#include "ThreadParserLoader.h"

#include <QtCore/QTimer>
#include <QtCore/QThread>
#include "ProgressIndicator.h"

ThreadParser::ThreadParser() : QObject(), CParser()
{
	synchronous = false;
	done = false;
	cancelled = false;
}


std::shared_ptr<CModel3D> ThreadParser::load(QString path, bool wait)
{
	synchronous = wait;

	fileinfo.setFile(path);

	if (fileinfo.exists())
	{
		loader = new ThreadParserLoader(fileinfo.absoluteFilePath());


		runLoader();
	}

	return nullptr;
}

bool ThreadParser::save(std::shared_ptr<CModel3D> obj, const QString path)
{
	return false;
}

void ThreadParser::runLoader()
{
	ProgressIndicator* progressIndicator = UI::PROGRESSBAR::instance();

	progressIndicator->init(0, 100, 0, "Reading " + fileinfo.fileName() + "...");

	connect(loader, SIGNAL(sendProgress(int)), progressIndicator, SLOT(setValue(int)));
	connect(progressIndicator->cancelButton(), SIGNAL(clicked()), loader, SLOT(cancelWork()));
	progressIndicator->cancelButton()->show();

	thread = new QThread();

	QTimer* timer = new QTimer();
	timer->setInterval(0);

	connect(loader, SIGNAL(workDone()), this, SLOT(onLoadDone()));
	connect(loader, SIGNAL(workDone()), timer, SLOT(stop()));
	connect(loader, SIGNAL(workDone()), progressIndicator, SLOT(hide()));

	connect(loader, SIGNAL(workCancelled()), this, SLOT(onLoadCancelled()));
	connect(loader, SIGNAL(workCancelled()), timer, SLOT(stop()));
	connect(loader, SIGNAL(workCancelled()), progressIndicator, SLOT(hide()));

	connect(timer, SIGNAL(timeout()), loader, SLOT(doWork()));

	connect(thread, SIGNAL(started()), loader, SLOT(createChild()));
	connect(thread, SIGNAL(started()), timer, SLOT(start()));

	connect(thread, SIGNAL(finished()), loader, SLOT(deleteLater()));
	connect(thread, SIGNAL(finished()), timer, SLOT(deleteLater()));
	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

	timer->moveToThread(thread);
	loader->moveToThread(thread);

	thread->setPriority(QThread::Priority::HighPriority);
	thread->start();
}

void ThreadParser::onLoadDone()
{
	done = true;

	thread->quit();
	while (!thread->isFinished());

	delete thread;

	UI::STATUSBAR::setText("File loading done!");
}

void ThreadParser::onLoadCancelled()
{
	cancelled = true;

	thread->quit();
	while (!thread->isFinished());

	delete thread;

	UI::STATUSBAR::setText("File loading cancelled!");
}

