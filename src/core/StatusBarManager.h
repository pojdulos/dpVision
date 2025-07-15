#pragma once
#include "interfaces/IStatusListener.h"

#include <cstdarg>
#include <string>
#include <QtCore/QString>

class StatusBarManager {
public:
    static void setListener(IStatusListener* l) { listener_ = l; }
    static IStatusListener* listener() { return listener_; }

    static void setText(const QString& text) {
        if (listener_) listener_->setText(text.toStdString());
    }

    //static void setText(const std::string& text) {
    //    if (listener_) listener_->setText(text);
    //}

    static void printfTimed(int mst, const QString& txt);

    static void printfTimed(int mst, const std::string& format, ...);

    static void clear() {
        if (listener_) listener_->clear();
    }
private:
    static IStatusListener* listener_;
};
