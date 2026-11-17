#pragma once

#include "../interfaces/IMessageBoxAPI.h"
#include "../../core/MessageBoxManager.h"

class MessageBoxAPIAdapter : public IMessageBoxAPI {
public:
    void information(const QString& message, const QString& title = QString()) override {
        MessageBoxManager::information(message.toStdString(), title.toStdString(), UserMessageChannel::Modal);
    }

    void warning(const QString& message, const QString& title = QString()) override {
        MessageBoxManager::warning(message.toStdString(), title.toStdString(), UserMessageChannel::Modal);
    }

    void error(const QString& message, const QString& title = QString()) override {
        MessageBoxManager::error(message.toStdString(), title.toStdString(), UserMessageChannel::Modal);
    }

    int question(
        const QString& message,
        const QString& title = QString(),
        const QString& button0 = QString("Yes"),
        const QString& button1 = QString("No"),
        const QString& button2 = QString()) override {
        return MessageBoxManager::question(
            message.toStdString(),
            title.toStdString(),
            button0.toStdString(),
            button1.toStdString(),
            button2.toStdString());
    }
};
