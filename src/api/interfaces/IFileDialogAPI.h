#pragma once

class QString;

class IFileDialogAPI {
public:
    virtual ~IFileDialogAPI() = default;
    virtual QString getOpenFileName(const QString& title, const QString& dir, const QString& filter) = 0;
    virtual QString getSaveFileName(const QString& title, const QString& dir, const QString& filter) = 0;
};
