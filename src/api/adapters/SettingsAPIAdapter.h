#pragma once

#include "../interfaces/ISettingsAPI.h"
#include "../../core/AppSettings.h"

class SettingsAPIAdapter : public ISettingsAPI {
public:
    QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const override {
        if (QSettings* settings = AppSettings::mainSettings()) {
            return settings->value(key, defaultValue);
        }
        return defaultValue;
    }

    void setValue(const QString& key, const QVariant& value) override {
        if (QSettings* settings = AppSettings::mainSettings()) {
            settings->setValue(key, value);
        }
    }

    bool contains(const QString& key) const override {
        if (QSettings* settings = AppSettings::mainSettings()) {
            return settings->contains(key);
        }
        return false;
    }

    void remove(const QString& key) override {
        if (QSettings* settings = AppSettings::mainSettings()) {
            settings->remove(key);
        }
    }

    void sync() override {
        if (QSettings* settings = AppSettings::mainSettings()) {
            settings->sync();
        }
    }
};
