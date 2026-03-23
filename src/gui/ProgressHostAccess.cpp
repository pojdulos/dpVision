#include "../api/adapters/ProgressHostAccess.h"

#include "../core/interfaces/IProgressListener.h"
#include "MainWindow.h"
#include "ProgressIndicator.h"
#include <QCoreApplication>

namespace {
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
    if (auto listener = IProgressListener::getDefault()) {
        listener->init(min, max, val);
    } else if (auto progress = instance()) {
        progress->init(min, max, val);
        progress->show();
    }
    processEvents();
}

void ProgressHostAccess::setValue(int val)
{
    if (auto listener = IProgressListener::getDefault()) {
        listener->setValue(val);
    } else if (auto progress = instance()) {
        progress->setValue(val);
    }
    processEvents();
}

void ProgressHostAccess::hide()
{
    if (auto listener = IProgressListener::getDefault()) {
        listener->hide();
    } else if (auto progress = instance()) {
        progress->hide();
    }
    processEvents();
}

void ProgressHostAccess::setText(const QString& text)
{
    if (auto listener = IProgressListener::getDefault()) {
        listener->setText(text.toStdString());
    } else if (auto progress = instance()) {
        progress->setText(text);
    }
    processEvents();
}
