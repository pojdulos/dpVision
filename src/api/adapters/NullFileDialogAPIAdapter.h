#pragma once

#include "../interfaces/IFileDialogAPI.h"
#include <QtCore/QString>

class NullFileDialogAPIAdapter : public IFileDialogAPI {
public:
    QString getOpenFileName(const QString& title, const QString& dir, const QString& filter) override {
        Q_UNUSED(title);
        Q_UNUSED(dir);
        Q_UNUSED(filter);
        return {};
    }

    QString getSaveFileName(const QString& title, const QString& dir, const QString& filter) override {
        Q_UNUSED(title);
        Q_UNUSED(dir);
        Q_UNUSED(filter);
        return {};
    }
};
