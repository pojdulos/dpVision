#pragma once

#include "../interfaces/IDockHistogramAPI.h"

class DockHistogramAPIAdapter : public IDockHistogramAPI {
public:
    void show(bool visible) override {
        visibleRef() = visible;
    }

    void setHistogram(CHistogram* histogram) override {
        histogramRef() = histogram;
    }

    CHistogram* getHistogram() override {
        return histogramRef();
    }

    void repaint() override {
    }

private:
    static CHistogram*& histogramRef() {
        static CHistogram* histogram = nullptr;
        return histogram;
    }

    static bool& visibleRef() {
        static bool visible = false;
        return visible;
    }
};
