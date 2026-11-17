#pragma once

class CHistogram;

class IHistogramDockListener {
public:
    virtual ~IHistogramDockListener() = default;

    virtual void show(bool visible) = 0;
    virtual void setHistogram(CHistogram* histogram) = 0;
    virtual CHistogram* getHistogram() = 0;
    virtual void repaint() = 0;
};
