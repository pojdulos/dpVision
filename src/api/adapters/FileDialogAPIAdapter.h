#pragma once

#include "../interfaces/IFileDialogAPI.h"
#include "../UI.h"

class FileDialogAPIAdapter : public IFileDialogAPI {
public:
    QString getOpenFileName(const QString& title, const QString& dir, const QString& filter) override {
        return UI::FILECHOOSER::getOpenFileName(title, dir, filter);
    }

    QString getSaveFileName(const QString& title, const QString& dir, const QString& filter) override {
        return UI::FILECHOOSER::getSaveFileName(title, dir, filter);
    }
};
