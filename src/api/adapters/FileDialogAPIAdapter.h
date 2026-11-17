#pragma once

#include "../interfaces/IFileDialogAPI.h"
#include <QFileDialog>
#include <QDir>

class FileDialogAPIAdapter : public IFileDialogAPI {
public:
    QString getOpenFileName(const QString& title, const QString& dir, const QString& filter) override {
        return QDir::toNativeSeparators(QFileDialog::getOpenFileName(nullptr, title, dir, filter));
    }

    QString getSaveFileName(const QString& title, const QString& dir, const QString& filter) override {
        return QDir::toNativeSeparators(QFileDialog::getSaveFileName(nullptr, title, dir, filter));
    }
};
