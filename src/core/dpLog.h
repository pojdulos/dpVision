#pragma once
#include <QDebug>
#include <QString>

class dpLogStream {
public:
    enum Level { Info, Warning, Error, Debug };

    dpLogStream(Level lvl) : level(lvl), str(), dbg(&str) {}

    ~dpLogStream() {
        QString color, prefix;
        switch (level) {
        case Info:    color = "\033[1;32m"; prefix = "[INFO] "; break;
        case Warning: color = "\033[1;33m"; prefix = "[WARN] "; break;
        case Error:   color = "\033[1;31m"; prefix = "[ERROR] "; break;
        case Debug:   color = "\033[1;34m"; prefix = "[DEBUG] "; break;
        }
        // Finalny wpis do stdout z kolorami
        fprintf(stdout, "%s%s%s\033[0m\n", color.toUtf8().constData(), prefix.toUtf8().constData(), str.toUtf8().constData());
        fflush(stdout);
    }

    template <typename T>
    dpLogStream& operator<<(const T& value) {
        dbg << value;
        return *this;
    }

    // manipulanty
    typedef dpLogStream& (*MyLogStreamManipulator)(dpLogStream&);
    dpLogStream& operator<<(MyLogStreamManipulator manip) { return manip(*this); }

private:
    Level level;
    QString str;
    QDebug dbg;
};

inline dpLogStream dpInfo() { return dpLogStream(dpLogStream::Info); }
inline dpLogStream dpWarn() { return dpLogStream(dpLogStream::Warning); }
inline dpLogStream dpError() { return dpLogStream(dpLogStream::Error); }
inline dpLogStream dpDebug() { return dpLogStream(dpLogStream::Debug); }
