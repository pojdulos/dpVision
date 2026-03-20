#pragma once

#include "../interfaces/IMessageBoxAPI.h"
#include "../../core/MessageBoxManager.h"

class MessageBoxAPIAdapter : public IMessageBoxAPI {
public:
    void information(const QString& message, const QString& title = QString()) override {
        MessageBoxManager::information(message.toStdString(), title.toStdString());
    }

    void warning(const QString& message, const QString& title = QString()) override {
        MessageBoxManager::warning(message.toStdString(), title.toStdString());
    }

    void error(const QString& message, const QString& title = QString()) override {
        MessageBoxManager::error(message.toStdString(), title.toStdString());
    }

    int question(
        const QString& message,
        const QString& title = QString(),
        const QString& button0 = QString("Yes"),
        const QString& button1 = QString("No"),
        const QString& button2 = QString()) override {
        Q_UNUSED(message);
        Q_UNUSED(title);
        Q_UNUSED(button0);
        Q_UNUSED(button1);
        Q_UNUSED(button2);
        return 0;
    }
};
