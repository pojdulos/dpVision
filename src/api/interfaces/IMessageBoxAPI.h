#pragma once

#include <QtCore/QString>

class IMessageBoxAPI {
public:
    virtual ~IMessageBoxAPI() = default;
    virtual void information(const QString& message, const QString& title = QString()) = 0;
    virtual void warning(const QString& message, const QString& title = QString()) = 0;
    virtual void error(const QString& message, const QString& title = QString()) = 0;
    virtual int question(
        const QString& message,
        const QString& title = QString(),
        const QString& button0 = QString("Yes"),
        const QString& button1 = QString("No"),
        const QString& button2 = QString()) = 0;
};
