#pragma once

#include <QObject>

#include "../../core/interfaces/IAppStateListener.h"
#include "../MainWindow.h"

class QtAppStateAdapter : public QObject, public IAppStateListener {
public:
    explicit QtAppStateAdapter(CMainWindow* window, QObject* parent = nullptr)
        : QObject(parent), window_(window)
    {
    }

    void updateProperties() override
    {
        if (window_ != nullptr && window_->dockProperties != nullptr) {
            window_->dockProperties->updateProperties();
            window_->dockProperties->update();
        }
    }

    void updateAllViews(bool buffered = true) override
    {
        if (window_ != nullptr) {
            window_->updateAllViews(buffered);
        }
    }

    void adjustForCurrentFile(const QString& filePath) override
    {
        if (window_ != nullptr) {
            window_->adjustForCurrentFile(filePath);
        }
    }

    void changeMenuAfterSelect() override
    {
        if (window_ != nullptr) {
            window_->changeMenuAfterSelect();
        }
    }

private:
    CMainWindow* window_;
};
