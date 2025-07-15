#include "StatusBarManager.h"
#include <QElapsedTimer>

IStatusListener* StatusBarManager::listener_ = nullptr;

void StatusBarManager::setTextTimed(int mst, const QString& txt) {
    if (listener_) {
        static QElapsedTimer timer;
        if (!timer.isValid())
            timer.start();

        if (timer.elapsed() > mst) {
            listener_->setText(txt.toStdString());
            timer.restart();
        }
    }
}

void StatusBarManager::printfTimed(int mst, const char* format, ...) {
   static QElapsedTimer timer;
   if (!timer.isValid())
       timer.start();

   if (timer.elapsed() > mst) {
       char formatBuf[1024];

       va_list paramList;
       va_start(paramList, format);

       vsnprintf(formatBuf, sizeof(formatBuf), format, paramList);
       va_end(paramList);

       setText(formatBuf);
       timer.restart();
   }
}

void StatusBarManager::printf(const char* format, ...) {
    char formatBuf[1024];

    va_list paramList;
    va_start(paramList, format);

    vsnprintf(formatBuf, sizeof(formatBuf), format, paramList);
    va_end(paramList);

    setText(formatBuf);
}
