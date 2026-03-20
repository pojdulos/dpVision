#pragma once
#include "dll_global.h"
#include "interfaces/IMessageListener.h"
#include "UserMessageManager.h"

#include <cstdarg>
#include <string>

class DPVISION_EXPORT MessageBoxManager {
public:
    static void setListener(IMessageListener* l) { UserMessageManager::setListener(l); }
    static IMessageListener* listener() { return UserMessageManager::listener(); }

    static void information(
        const std::string &msg,
        const std::string &tittle = "",
        UserMessageChannel channel = UserMessageChannel::Log) {
        UserMessageManager::information(msg, tittle, channel);
    }
    static void warning(
        const std::string &msg,
        const std::string &tittle = "",
        UserMessageChannel channel = UserMessageChannel::Log) {
        UserMessageManager::warning(msg, tittle, channel);
    }
    static void error(
        const std::string &msg,
        const std::string &tittle = "",
        UserMessageChannel channel = UserMessageChannel::Log) {
        UserMessageManager::error(msg, tittle, channel);
    }
    static int question(
        const std::string& msg,
        const std::string& tittle = "",
        const std::string& b0 = "Yes",
        const std::string& b1 = "No",
        const std::string& b2 = "",
        UserMessageChannel channel = UserMessageChannel::Modal) {
        return UserMessageManager::question(msg, tittle, b0, b1, b2, channel);
    }
};
