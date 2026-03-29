#pragma once

#include "dll_global.h"
#include "interfaces/IAppStateListener.h"

class QString;

class DPVISION_EXPORT AppStateManager {
public:
    static void setListener(IAppStateListener* listener) { listenerRef() = listener; }
    static IAppStateListener* listener() { return listenerRef(); }

    static void updateProperties() {
        if (listenerRef()) {
            listenerRef()->updateProperties();
        }
    }

    static void updateView(bool repaintAll = true, bool buffered = true) {
        if (listenerRef()) {
            listenerRef()->updateView(repaintAll, buffered);
        }
    }

    static void updateAllViews(bool buffered = true) {
        if (listenerRef()) {
            listenerRef()->updateAllViews(buffered);
        }
    }

    static void adjustForCurrentFile(const QString& filePath) {
        if (listenerRef()) {
            listenerRef()->adjustForCurrentFile(filePath);
        }
    }

    static void changeMenuAfterSelect() {
        if (listenerRef()) {
            listenerRef()->changeMenuAfterSelect();
        }
    }

private:
    static IAppStateListener*& listenerRef() {
        static IAppStateListener* listener = nullptr;
        return listener;
    }
};
