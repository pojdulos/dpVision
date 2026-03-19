#pragma once

#include "../interfaces/IMessageBoxAPI.h"
#include "../UI.h"

class MessageBoxAPIAdapter : public IMessageBoxAPI {
public:
    void information(const QString& message, const QString& title = QString()) override {
        UI::MESSAGEBOX::information(message, title);
    }

    void warning(const QString& message, const QString& title = QString()) override {
        UI::MESSAGEBOX::warning(message, title);
    }

    void error(const QString& message, const QString& title = QString()) override {
        UI::MESSAGEBOX::error(message, title);
    }

    int question(
        const QString& message,
        const QString& title = QString(),
        const QString& button0 = QString("Yes"),
        const QString& button1 = QString("No"),
        const QString& button2 = QString()) override {
        return UI::MESSAGEBOX::question(message, title, button0, button1, button2);
    }
};
