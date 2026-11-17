#pragma once

#include "dll_global.h"
#include "interfaces/IMessageListener.h"

#include <string>

class DPVISION_EXPORT UserMessageManager {
public:
    static void setListener(IMessageListener* l) { listenerRef() = l; }
    static IMessageListener* listener() { return listenerRef(); }

    static void post(const UserMessage& message) {
        if (listenerRef()) {
            listenerRef()->notify(message);
        }
    }

    static int ask(const UserQuestion& question) {
        if (listenerRef()) {
            return listenerRef()->question(question);
        }
        return 0;
    }

    static void information(
        const std::string& msg,
        const std::string& title = "",
        UserMessageChannel channel = UserMessageChannel::Log) {
        UserMessage message;
        message.severity = UserMessageSeverity::Info;
        message.channel = channel;
        message.title = title;
        message.text = msg;
        post(message);
    }

    static void warning(
        const std::string& msg,
        const std::string& title = "",
        UserMessageChannel channel = UserMessageChannel::Log) {
        UserMessage message;
        message.severity = UserMessageSeverity::Warning;
        message.channel = channel;
        message.title = title;
        message.text = msg;
        post(message);
    }

    static void error(
        const std::string& msg,
        const std::string& title = "",
        UserMessageChannel channel = UserMessageChannel::Log) {
        UserMessage message;
        message.severity = UserMessageSeverity::Error;
        message.channel = channel;
        message.title = title;
        message.text = msg;
        post(message);
    }

    static int question(
        const std::string& msg,
        const std::string& title = "",
        const std::string& button0 = "Yes",
        const std::string& button1 = "No",
        const std::string& button2 = "",
        UserMessageChannel channel = UserMessageChannel::Modal) {
        UserQuestion questionData;
        questionData.channel = channel;
        questionData.title = title;
        questionData.text = msg;
        questionData.button0 = button0;
        questionData.button1 = button1;
        questionData.button2 = button2;
        return ask(questionData);
    }

private:
    static IMessageListener*& listenerRef();
};
