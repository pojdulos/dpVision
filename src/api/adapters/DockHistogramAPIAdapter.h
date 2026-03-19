#pragma once

#include "../interfaces/IDockHistogramAPI.h"

class DockHistogramAPIAdapter : public IDockHistogramAPI {
public:
    void show(bool visible) override {
        visible_ = visible;
    }

    void setHistogram(CHistogram* histogram) override {
        histogram_ = histogram;
    }

    CHistogram* getHistogram() override {
        return histogram_;
    }

    void repaint() override {
    }

private:
    inline static CHistogram* histogram_ = nullptr;
    inline static bool visible_ = false;
};
