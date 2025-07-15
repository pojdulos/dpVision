#pragma once
#include "interfaces/IMessageListener.h"

#include <cstdarg>
#include <string>

class MessageBoxManager {
public:
    static void setListener(IMessageListener* l) { listener_ = l; }
    static IMessageListener* listener() { return listener_; }

    void information(const std::string &msg, const std::string &tittle) {
        if (listener_) listener_->information(msg, tittle);
    }
    void warning(const std::string &msg, const std::string &tittle) {
        if (listener_) listener_->warning(msg, tittle);
    }
    void error(const std::string &msg, const std::string &tittle) {
        if (listener_) listener_->error(msg, tittle);
    }
    //int question(const std::string &msg, const std::string &tittle, const std::string &b0, const std::string &b1, const std::string &b2) {
    //    if (listener_) return listener_->question(msg, tittle, b0, b1, b2);
    //    else return 0;
    //}

    private:
    static IMessageListener* listener_;
};
