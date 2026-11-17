#include "QtStatusBarAdapter.h"

#include <QMetaObject>
#include <QString>
#include <QStatusBar>

QtStatusBarAdapter::QtStatusBarAdapter(QStatusBar* bar)
    : bar_(bar)
{
}

void QtStatusBarAdapter::setText(const std::string& text)
{
    if (bar_) {
        const QString message = QString::fromStdString(text);
        QMetaObject::invokeMethod(bar_, [bar = bar_, message]() {
            if (bar) {
                bar->showMessage(message);
            }
        }, Qt::QueuedConnection);
    }
}

void QtStatusBarAdapter::clear()
{
    if (bar_) {
        QMetaObject::invokeMethod(bar_, [bar = bar_]() {
            if (bar) {
                bar->clearMessage();
            }
        }, Qt::QueuedConnection);
    }
}
