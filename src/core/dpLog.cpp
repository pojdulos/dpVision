#include "dpLog.h"

#include <QDateTime>
#include <QDebug>

#include <QDateTime>
#include <QDebug>

// --- Wewnêtrzne statyczne zmienne ---
static QFile* s_logFile = nullptr;
static QTextStream* s_logStream = nullptr;
static QMutex s_mutex;
static bool s_loggingEnabled = true;


dpLogStream::dpLogStream(dpLogger::Level lvl)
    : buffer(), stream(&buffer, QIODevice::WriteOnly), level(lvl)
{
}

dpLogStream::~dpLogStream()
{
    if (!buffer.isEmpty()) {
        dpLogger::output(level, buffer);
    }
}


// --- Implementacja ---
dpLogStream dpLogger::info() { return dpLogStream(Info); }
dpLogStream dpLogger::warn() { return dpLogStream(Warning); }
dpLogStream dpLogger::error() { return dpLogStream(Error); }
dpLogStream dpLogger::debug() { return dpLogStream(Debug); }

void dpLogger::setLogFile(const QString& filePath)
{
    QMutexLocker locker(&mutex());
    if (s_logFile) {
        s_logFile->close();
        delete s_logStream;
        delete s_logFile;
        s_logFile = nullptr;
        s_logStream = nullptr;
    }
    s_logFile = new QFile(filePath);
    if (s_logFile->open(QIODevice::Append | QIODevice::Text)) {
        s_logStream = new QTextStream(s_logFile);
    }
}

void dpLogger::setLoggingEnabled(bool enabled)
{
    QMutexLocker locker(&mutex());
    loggingEnabled() = enabled;
}

void dpLogger::output(Level level, const QString& msg)
{
    QMutexLocker locker(&mutex());
    if (!loggingEnabled()) return;

    QString levelStr;
    switch (level) {
    case Info:    levelStr = "INFO";    break;
    case Warning: levelStr = "WARNING"; break;
    case Error:   levelStr = "ERROR";   break;
    case Debug:   levelStr = "DEBUG";   break;
    }
    QString timeStr = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString fullMsg = QString("[%1] [%2] %3").arg(timeStr, levelStr, msg);

    // Konsola (bez cudzys³owów)
    qInfo().noquote() << fullMsg;

    // Plik
    if (s_logStream) {
        (*s_logStream) << fullMsg << "\n";
        s_logStream->flush();
    }
}

// --- Wewnêtrzne gettery statyczne (do u¿ycia w destruktorze streamu) ---
QFile* dpLogger::logFile() { return s_logFile; }
QTextStream* dpLogger::logStream() { return s_logStream; }
QMutex& dpLogger::mutex() { return s_mutex; }
bool& dpLogger::loggingEnabled() { return s_loggingEnabled; }
