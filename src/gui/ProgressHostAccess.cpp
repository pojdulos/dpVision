#include "ProgressHostAccess.h"

#include "../core/interfaces/IProgressListener.h"
#include "MainWindow.h"
#include "ProgressIndicator.h"
#include <QCoreApplication>

namespace {
std::shared_ptr<IProgressListener> defaultProgressListener()
{
    return IProgressListener::getDefault();
}

void processEvents()
{
    QCoreApplication::processEvents();
}
}

ProgressIndicator* ProgressHostAccess::instance()
{
    if (auto win = CMainWindow::instance()) {
        return win->progressIndicator;
    }
    return nullptr;
}

void ProgressHostAccess::init(int min, int max, int val)
{
    if (auto listener = defaultProgressListener()) {
        listener->init(min, max, val);
    }
    processEvents();
}

void ProgressHostAccess::setValue(int val)
{
    if (auto listener = defaultProgressListener()) {
        listener->setValue(val);
    }
    processEvents();
}

void ProgressHostAccess::hide()
{
    if (auto listener = defaultProgressListener()) {
        listener->hide();
    }
    processEvents();
}

void ProgressHostAccess::setText(const QString& text)
{
    if (auto listener = defaultProgressListener()) {
        listener->setText(text.toStdString());
    }
    processEvents();
}
