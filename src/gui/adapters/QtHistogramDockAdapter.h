#pragma once

#include <QObject>

#include "../../core/interfaces/IHistogramDockListener.h"
#include "../MainWindow.h"

class QtHistogramDockAdapter : public QObject, public IHistogramDockListener {
public:
    explicit QtHistogramDockAdapter(CMainWindow* window, QObject* parent = nullptr)
        : QObject(parent), window_(window)
    {
    }

    void show(bool visible) override
    {
        if (window_ != nullptr && window_->dockHisto != nullptr) {
            window_->dockHisto->setVisible(visible);
        }
    }

    void setHistogram(CHistogram* histogram) override
    {
        if (window_ != nullptr && window_->dockHisto != nullptr) {
            window_->dockHisto->setHistogram(histogram);
        }
    }

    CHistogram* getHistogram() override
    {
        if (window_ != nullptr && window_->dockHisto != nullptr) {
            return window_->dockHisto->getHistogram();
        }
        return nullptr;
    }

    void repaint() override
    {
        if (window_ != nullptr && window_->dockHisto != nullptr) {
            window_->dockHisto->update();
        }
    }

private:
    CMainWindow* window_;
};
