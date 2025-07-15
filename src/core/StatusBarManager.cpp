#include "StatusBarManager.h"
#include <QElapsedTimer>

IStatusListener* StatusBarManager::listener_ = nullptr;

void StatusBarManager::printfTimed(int mst, const QString& txt) {
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

//void StatusBarManager::printfTimed(int mst, const std::string& format, ...) {
//    static QElapsedTimer timer;
//    if (!timer.isValid())
//        timer.start();
//
//    if (timer.elapsed() > mst) {
//        char formatBuf[1024];
//
//        va_list paramList;
//        va_start(paramList, format.c_str());
//        vsnprintf(formatBuf, sizeof(formatBuf), format.c_str(), paramList);
//        va_end(paramList);
//
//        setText(formatBuf);
//        timer.restart();
//    }
//}
