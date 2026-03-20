#pragma once

#include <QVariant>

class QString;

class ISettingsAPI {
public:
    virtual ~ISettingsAPI() = default;
    virtual QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const = 0;
    virtual void setValue(const QString& key, const QVariant& value) = 0;
    virtual bool contains(const QString& key) const = 0;
    virtual void remove(const QString& key) = 0;
    virtual void sync() = 0;
};
