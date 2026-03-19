#pragma once
#include "dll_global.h"
#include "interfaces/IStatusListener.h"

#include <cstdarg>
#include <string>
#include <QtCore/QString>

class DPVISION_EXPORT StatusBarManager {
public:
    static void setListener(IStatusListener* l) { listenerRef() = l; }
    static IStatusListener* listener() { return listenerRef(); }

    static void setText(const QString& text) {
        if (listenerRef()) listenerRef()->setText(text.toStdString());
    }

    //static void setText(const std::string& text) {
    //    if (listener_) listener_->setText(text);
    //}

    static void setTextTimed(int mst, const QString& txt);

    static void printf(const char* format, ...);
    static void printfTimed(int mst, const char* format, ...);

    static void clear() {
        if (listenerRef()) listenerRef()->clear();
    }
private:
    static IStatusListener*& listenerRef();
};
