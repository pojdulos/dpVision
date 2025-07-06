#pragma once
#ifndef _LOG_H_BY_DP_INCLUDED_
#define _LOG_H_BY_DP_INCLUDED_

#include "MainApplication.h"
#include "AP.h"

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMutex>

class dpLogStream;

class DPVISION_EXPORT dpLogger
{
public:
    enum Level { Info, Warning, Error, Debug };

    static dpLogStream info();
    static dpLogStream warn();
    static dpLogStream error();
    static dpLogStream debug();

    static void setLogFile(const QString& filePath);
    static void setLoggingEnabled(bool enabled);

private:
    friend class dpLogStream;
    static void output(Level level, const QString& msg);

    static QFile* logFile();
    static QTextStream* logStream();
    static QMutex& mutex();
    static bool& loggingEnabled();
};


class DPVISION_EXPORT dpLogStream
{
public:
    dpLogStream(dpLogger::Level lvl);
    ~dpLogStream();

    template<typename T>
    dpLogStream& operator<<(const T& value) {
        stream << value;
        return *this;
    }

    // Obs³uga np. std::endl
    typedef dpLogStream& (*MyLogStreamManipulator)(dpLogStream&);
    dpLogStream& operator<<(MyLogStreamManipulator manip) { return manip(*this); }

private:
    QString buffer;
    QTextStream stream;
    dpLogger::Level level;
};

inline dpLogStream dpInfo() { return dpLogger::info(); }
inline dpLogStream dpWarn() { return dpLogger::warn(); }
inline dpLogStream dpError() { return dpLogger::error(); }
inline dpLogStream dpDebug() { return dpLogger::debug(); }

#endif