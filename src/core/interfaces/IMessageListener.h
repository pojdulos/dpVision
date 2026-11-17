#pragma once

#include "UserMessage.h"

#include <string>

class IMessageListener {
public:
    virtual ~IMessageListener() = default;
    virtual void notify(const UserMessage& message) = 0;
    virtual int question(const UserQuestion& question) = 0;

    virtual void information(const std::string &msg, const std::string &tittle) {
        UserMessage message;
        message.severity = UserMessageSeverity::Info;
        message.title = tittle;
        message.text = msg;
        notify(message);
    }

    virtual void warning(const std::string &msg, const std::string &tittle) {
        UserMessage message;
        message.severity = UserMessageSeverity::Warning;
        message.title = tittle;
        message.text = msg;
        notify(message);
    }

    virtual void error(const std::string &msg, const std::string &tittle) {
        UserMessage message;
        message.severity = UserMessageSeverity::Error;
        message.title = tittle;
        message.text = msg;
        notify(message);
    }
};
