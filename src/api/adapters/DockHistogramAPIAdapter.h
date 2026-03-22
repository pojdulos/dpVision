#pragma once

#include "../../core/HistogramDockManager.h"
#include "../interfaces/IDockHistogramAPI.h"

class DockHistogramAPIAdapter : public IDockHistogramAPI {
public:
    void show(bool visible) override {
        HistogramDockManager::show(visible);
    }

    void setHistogram(CHistogram* histogram) override {
        HistogramDockManager::setHistogram(histogram);
    }

    CHistogram* getHistogram() override {
        return HistogramDockManager::getHistogram();
    }

    void repaint() override {
        HistogramDockManager::repaint();
    }
};
