#pragma once

#include "dll_global.h"
#include "interfaces/IHistogramDockListener.h"

class DPVISION_EXPORT HistogramDockManager {
public:
    static void setListener(IHistogramDockListener* listener) { listenerRef() = listener; }
    static IHistogramDockListener* listener() { return listenerRef(); }

    static void show(bool visible) {
        if (listenerRef() != nullptr) {
            listenerRef()->show(visible);
        }
    }

    static void setHistogram(CHistogram* histogram) {
        if (listenerRef() != nullptr) {
            listenerRef()->setHistogram(histogram);
        }
    }

    static CHistogram* getHistogram() {
        if (listenerRef() != nullptr) {
            return listenerRef()->getHistogram();
        }
        return nullptr;
    }

    static void repaint() {
        if (listenerRef() != nullptr) {
            listenerRef()->repaint();
        }
    }

private:
    static IHistogramDockListener*& listenerRef() {
        static IHistogramDockListener* listener = nullptr;
        return listener;
    }
};
