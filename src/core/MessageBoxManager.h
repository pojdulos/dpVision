#pragma once
#include "dll_global.h"
#include "interfaces/IMessageListener.h"

#include <cstdarg>
#include <string>

class DPVISION_EXPORT MessageBoxManager {
public:
    static void setListener(IMessageListener* l) { listenerRef() = l; }
    static IMessageListener* listener() { return listenerRef(); }

    static void information(const std::string &msg, const std::string &tittle = "") {
        if (listenerRef()) listenerRef()->information(msg, tittle);
    }
    static void warning(const std::string &msg, const std::string &tittle = "") {
        if (listenerRef()) listenerRef()->warning(msg, tittle);
    }
    static void error(const std::string &msg, const std::string &tittle = "") {
        if (listenerRef()) listenerRef()->error(msg, tittle);
    }
    //int question(const std::string &msg, const std::string &tittle, const std::string &b0, const std::string &b1, const std::string &b2) {
    //    if (listener_) return listener_->question(msg, tittle, b0, b1, b2);
    //    else return 0;
    //}

    private:
    static IMessageListener*& listenerRef();
};
