#pragma once

#include <QObject>
#include <QString>
#include <QMessageBox>
#include <QDebug>
#include "interfaces/IMessageListener.h"
#include "../../core/StatusBarManager.h"

class QtMessageBoxAdapter : public QObject, public IMessageListener {
    Q_OBJECT
public:
    QtMessageBoxAdapter() {}
    void notify(const UserMessage& message) override {
        const QString title = QString::fromUtf8(message.title.c_str());
        const QString text = QString::fromUtf8(message.text.c_str());

        switch (message.channel) {
        case UserMessageChannel::Status:
            StatusBarManager::setText(text);
            return;
        case UserMessageChannel::Auto:
        case UserMessageChannel::Log:
            log(message);
            return;
        case UserMessageChannel::Popup:
            log(message);
            break;
        case UserMessageChannel::Modal:
            log(message);
            break;
        default:
            break;
        }

        switch (message.severity) {
        case UserMessageSeverity::Info:
            QMessageBox::information(nullptr, title, text);
            break;
        case UserMessageSeverity::Warning:
            QMessageBox::warning(nullptr, title, text);
            break;
        case UserMessageSeverity::Error:
            QMessageBox::critical(nullptr, title, text);
            break;
        }
    }

    int question(const UserQuestion& questionData) override {
        const QString title = QString::fromUtf8(questionData.title.c_str());
        const QString text = QString::fromUtf8(questionData.text.c_str());
        const QString b0 = QString::fromUtf8(questionData.button0.c_str());
        const QString b1 = QString::fromUtf8(questionData.button1.c_str());
        const QString b2 = QString::fromUtf8(questionData.button2.c_str());

        if (questionData.channel == UserMessageChannel::Log) {
            qWarning().noquote() << text;
            return 0;
        }

        if (questionData.channel == UserMessageChannel::Status) {
            StatusBarManager::setText(text);
            return 0;
        }

        return QMessageBox::question(nullptr, title, text, b0, b1, b2);
    }

private:
    void log(const UserMessage& message) {
        const QString text = QString::fromUtf8(message.text.c_str());
        switch (message.severity) {
        case UserMessageSeverity::Info:
            qInfo().noquote() << text;
            break;
        case UserMessageSeverity::Warning:
            qWarning().noquote() << text;
            break;
        case UserMessageSeverity::Error:
            qCritical().noquote() << text;
            break;
        }
    }
};
