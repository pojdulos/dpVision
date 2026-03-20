#pragma once

#include "../interfaces/IProgressAPI.h"
#include "../../gui/MainWindow.h"
#include "../../gui/ProgressIndicator.h"

class GuiProgressAPIAdapter : public IProgressAPI {
public:
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
        AP::processEvents(true);
    }

    void setValue(int val) override {
        if (auto progress = instance()) {
            progress->setValue(val);
        }
        AP::processEvents(true);
    }

    void hide() override {
        if (auto progress = instance()) {
            progress->hide();
        }
        AP::processEvents(true);
    }

    void setText(const QString& text) override {
        if (auto progress = instance()) {
            progress->setText(text);
        }
        AP::processEvents(true);
    }
};
