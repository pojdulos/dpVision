#pragma once

#include "../interfaces/IStatusBarAPI.h"
#include "../../gui/MainWindow.h"
#include "../AP.h"

class StatusBarAPIAdapter : public IStatusBarAPI {
public:
    void setText(const QString& message) override {
        if (auto win = CMainWindow::instance()) {
            if (auto sb = win->statusBar()) {
                sb->showMessage(message);
            }
        }

        AP::processEvents(true);
    }

    void setTimedText(int mst, const QString& message) override {
        if (auto win = CMainWindow::instance()) {
            if (auto sb = win->statusBar()) {
                sb->showMessage(message, mst);
            }
        }

        AP::processEvents(true);
    }
};
