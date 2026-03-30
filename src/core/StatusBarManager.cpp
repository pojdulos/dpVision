#include "StatusBarManager.h"
#include <QElapsedTimer>

IStatusListener*& StatusBarManager::listenerRef()
{
    static IStatusListener* listener = nullptr;
    return listener;
}

void StatusBarManager::setTextTimed(int mst, const QString& txt) {
    if (listenerRef()) {
        static QElapsedTimer timer;
        if (!timer.isValid())
            timer.start();

        if (timer.elapsed() > mst) {
            listenerRef()->setText(txt.toStdString());
            timer.restart();
        }
    }
}

void StatusBarManager::printfTimed(int mst, const char* format, ...) {
    va_list paramList;
    va_start(paramList, format);
    vprintfTimed(mst, format, paramList);
    va_end(paramList);
}

void StatusBarManager::vprintf(const char* format, va_list args) {
    char formatBuf[1024];
    va_list argsCopy;
    va_copy(argsCopy, args);
    vsnprintf(formatBuf, sizeof(formatBuf), format, argsCopy);
    va_end(argsCopy);
    setText(formatBuf);
}

void StatusBarManager::printf(const char* format, ...) {
    va_list paramList;
    va_start(paramList, format);
    vprintf(format, paramList);
    va_end(paramList);
}

void StatusBarManager::vprintf(const wchar_t* format, va_list args) {
    wchar_t formatBuf[1024];
    va_list argsCopy;
    va_copy(argsCopy, args);
    vswprintf(formatBuf, sizeof(formatBuf) / sizeof(formatBuf[0]), format, argsCopy);
    va_end(argsCopy);
    setText(QString::fromWCharArray(formatBuf));
}

void StatusBarManager::printf(const wchar_t* format, ...) {
    va_list paramList;
    va_start(paramList, format);
    vprintf(format, paramList);
    va_end(paramList);
}

void StatusBarManager::vprintfTimed(int mst, const char* format, va_list args) {
    static QElapsedTimer timer;
    if (!timer.isValid())
        timer.start();

    if (timer.elapsed() > mst) {
        char formatBuf[1024];
        va_list argsCopy;
        va_copy(argsCopy, args);
        vsnprintf(formatBuf, sizeof(formatBuf), format, argsCopy);
        va_end(argsCopy);
        setText(formatBuf);
        timer.restart();
    }
}

void StatusBarManager::printfTimed(int mst, const wchar_t* format, ...) {
    va_list paramList;
    va_start(paramList, format);
    vprintfTimed(mst, format, paramList);
    va_end(paramList);
}

void StatusBarManager::vprintfTimed(int mst, const wchar_t* format, va_list args) {
    static QElapsedTimer timer;
    if (!timer.isValid())
        timer.start();

    if (timer.elapsed() > mst) {
        wchar_t formatBuf[1024];
        va_list argsCopy;
        va_copy(argsCopy, args);
        vswprintf(formatBuf, sizeof(formatBuf) / sizeof(formatBuf[0]), format, argsCopy);
        va_end(argsCopy);
        setText(QString::fromWCharArray(formatBuf));
        timer.restart();
    }
}
