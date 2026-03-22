#pragma once

#include "../interfaces/IProgressAPI.h"
#include "../../gui/MainWindow.h"
#include "../../gui/ProgressIndicator.h"
#include <QCoreApplication>

class GuiProgressAPIAdapter : public IProgressAPI {
public:
    void processEvents()
    {
        QCoreApplication::processEvents();
    }

    ProgressIndicator* instance() override {
        if (auto win = CMainWindow::instance()) {
            return win->progressIndicator;
        }
        return nullptr;
    }

    void init(int min, int max, int val) override {
        if (auto progress = instance()) {
            progress->init(min, max, val);
            progress->show();
        }
        processEvents();
    }

    void setValue(int val) override {
        if (auto progress = instance()) {
            progress->setValue(val);
        }
        processEvents();
    }

    void hide() override {
        if (auto progress = instance()) {
            progress->hide();
        }
        processEvents();
    }

    void setText(const QString& text) override {
        if (auto progress = instance()) {
            progress->setText(text);
        }
        processEvents();
    }
};
