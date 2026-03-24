#include "LegacyAppRuntime.h"

#include <QCoreApplication>
#include <QElapsedTimer>

void LegacyAppRuntime::processEvents(bool immediate)
{
    static QElapsedTimer timer;
    if (!timer.isValid()) {
        timer.start();
    }

    if (immediate || timer.elapsed() > 1000) {
        QCoreApplication::processEvents();
        timer.restart();
    }
}
